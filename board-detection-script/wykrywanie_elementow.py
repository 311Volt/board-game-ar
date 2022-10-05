import os
import cv2
import numpy as np

from przydatne_funkcje import przeskaluj
from przydatne_funkcje import wyswietl_lub_zapisz


# skala_do_wykrywania - ma zmniejszać obrazek, więc musi być ułamkiem
def wykryj_plansze(plansza, skala_do_wykrywania=1.0, wyswietlaj_kroki=False, zapisuj_kroki=False, sciezka_wyniki=None):
    # zmniejszanie obrazka na etap dobierania odpowiedniej maski - dla lepszego
    # wykrycia planszy i wydajniejszego przetwarzania
    plansza_mniejsza = przeskaluj(plansza, skala_do_wykrywania)
    plansza_thresh = obrobka_obrazka_przed_wykrywaniem_konturow(plansza_mniejsza, wyswietlaj_kroki,
                                                                zapisuj_kroki, sciezka_wyniki)

    kontury, hierarchia = cv2.findContours(plansza_thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    # sprawdzenie jak wykryły się kontury na obrazku
    plansza_kontury = plansza_mniejsza.copy()
    cv2.drawContours(plansza_kontury, kontury, -1, (0, 255, 0), thickness=2)
    wyswietl_lub_zapisz(plansza_kontury, 'Krok 5 - '+'Plansza kontury', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    skalowanie_konturow(kontury, 1.0/skala_do_wykrywania)
    maska = dobieranie_maski_dla_obrazka(kontury, plansza, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    glebia_obrazka = 1 if len(plansza.shape) <= 2 else plansza.shape[2]
    zmaskowana_plansza = plansza & np.stack(([maska]*glebia_obrazka), 2)  # maska 2d rozszerzona w osi z do takiej głębi jaką ma obrazek
    wyswietl_lub_zapisz(zmaskowana_plansza, 'Krok 8 - ' + 'Zmaskowana plansza', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    # dodanie kanału alfa do obrazka (przezroczystości) ###
    zmaskowana_plansza = cv2.cvtColor(zmaskowana_plansza, cv2.COLOR_BGR2BGRA)
    zmaskowana_plansza[:, :, 3] = zmaskowana_plansza[:, :, 3] & maska
    #cv2.imwrite(sciezka_wyniki+'\\'+'Krok 9 - Kanal przezroczystosci.png', zmaskowana_plansza)

    przycieta_plansza = wycinanie_obrazka(zmaskowana_plansza, maska, plansza, wyswietlaj_kroki, zapisuj_kroki,
                                          sciezka_wyniki)

    if wyswietlaj_kroki:
        cv2.waitKey(0)

    return przycieta_plansza


# obróbka obrazka, żeby jak najdokładniej wykryła się plansza
def obrobka_obrazka_przed_wykrywaniem_konturow(plansza_mniejsza, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    b, g, r = cv2.split(plansza_mniejsza)
    wyswietl_lub_zapisz(b, 'Krok 1 - ' + 'Plansza niebieski kanal', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    plansza_zrown = cv2.equalizeHist(b)
    wyswietl_lub_zapisz(plansza_zrown, 'Krok 2 - ' + 'Plansza zrown', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    plansza_rozmyta = cv2.medianBlur(plansza_zrown, 15)  # było 15 dla małej planszy
    wyswietl_lub_zapisz(plansza_rozmyta, 'Krok 3 - ' + 'Plansza rozmyta', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    ret, plansza_thresh = cv2.threshold(plansza_rozmyta, 200, 255, cv2.THRESH_BINARY)
    wyswietl_lub_zapisz(plansza_thresh, 'Krok 4 - ' + 'Plansza thresh', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    return plansza_thresh


def dobieranie_maski_dla_obrazka(kontury, plansza, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    # maska = np.zeros((plansza_mniejsza.shape[0], plansza_mniejsza.shape[1], 3), dtype='uint8')
    maska = np.zeros((plansza.shape[0], plansza.shape[1]), dtype='uint8')
    wyswietl_lub_zapisz(maska, 'Krok 6 - ' + 'Pusty', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    cv2.drawContours(maska, kontury, -1, (255, 255, 255), thickness=-1)
    wyswietl_lub_zapisz(maska, 'Krok 7 - ' + 'Maska', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    return maska


def wycinanie_obrazka(zmaskowana_plansza, maska, plansza, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    # wycinanie planszy - znalezienie białych punktów i wybranie najbardziej wysuniętych punktów wykrytego kształtu
    y, x = np.where(maska == 255)
    (najmniejszy_y, najmniejszy_x) = (np.min(y), np.min(x))
    (najwiekszy_y, najwiekszy_x) = (np.max(y), np.max(x))

    # test gdzie się wykryły punkty graniczne
    kopia_planszy = plansza.copy()
    cv2.rectangle(kopia_planszy, (najmniejszy_x, najmniejszy_y), (najwiekszy_x, najwiekszy_y), (0, 255, 0), thickness=3)
    wyswietl_lub_zapisz(kopia_planszy, 'Krok 10 - ' + 'Test przycinania', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    # wycięcie planszy
    przycieta_plansza = zmaskowana_plansza[najmniejszy_y:najwiekszy_y, najmniejszy_x:najwiekszy_x]

    return przycieta_plansza


# poprawić, żeby zwracało kopię przeskalowanych konturów, a nie zmieniało oryginalne
def skalowanie_konturow(kontury, skala):
    for kontur in kontury:
        kontur[:, :, 0] = kontur[:, :, 0] * skala  # skalowanie x
        kontur[:, :, 1] = kontur[:, :, 1] * skala  # skalowanie y
