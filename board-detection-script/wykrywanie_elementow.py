import cv2
import numpy as np

from pomocnicze_funkcje_do_obrazkow import przeskaluj
from pomocnicze_funkcje_do_obrazkow import wyswietl_lub_zapisz
from pomocnicze_funkcje_do_obrazkow import wyrownaj_histogram_obrazka
from pomocnicze_funkcje_do_obrazkow import skalowanie_konturow
from pomocnicze_funkcje_do_obrazkow import znajdz_duze_kontury
from pomocnicze_funkcje_do_obrazkow import znajdz_najwiekszy_kontur


# skala_do_wykrywania - ma zmniejszać obrazek, więc musi być ułamkiem
def wykryj_plansze(plansza, skala_do_wykrywania=1.0, wyswietlaj_kroki=False, zapisuj_kroki=False, sciezka_wyniki=None):
    # zmniejszanie obrazka na etap dobierania odpowiedniej maski - dla lepszego
    # wykrycia planszy i wydajniejszego przetwarzania
    plansza_mniejsza = przeskaluj(plansza, skala_do_wykrywania)

    # przedział niebieskości ramki planszy
    # najwezej 120, 80, 0  # szersze 90, 60, 0  # dla wyrownanego histogramu 160, 90, 0
    ograniczenie_dolne = np.array([160, 90, 0], dtype='uint8')
    # najwezej 175, 130, 50  # szersze 175, 150, 130  # dla wyrownanego histogramu 255, 200, 100
    ograniczenie_gorne = np.array([255, 200, 100], dtype='uint8')
    przedzial_niebieskosci = [ograniczenie_dolne, ograniczenie_gorne]
    plansza_thresh = obrobka_obrazka_przed_wykrywaniem_konturow_po_kolorze(plansza_mniejsza, przedzial_niebieskosci,
                                                                           wyswietlaj_kroki, zapisuj_kroki,
                                                                           sciezka_wyniki)
    # poprzedni sposób
    #plansza_thresh = obrobka_obrazka_przed_wykrywaniem_konturow_po_wybranym_kanale(plansza_mniejsza, 0,
    #                                                                               wyswietlaj_kroki, zapisuj_kroki,
    #                                                                               sciezka_wyniki)

    # wykrywanie konturów na obrazku
    kontury, hierarchia = cv2.findContours(plansza_thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    # sprawdzenie jak wykryły się kontury na obrazku
    plansza_kontury = plansza_mniejsza.copy()
    cv2.drawContours(plansza_kontury, kontury, -1, (0, 255, 0), thickness=2)
    wyswietl_lub_zapisz(plansza_kontury, 'Krok 5 - '+'Plansza kontury', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    # przeskalowanie konturów do rozmiarów oryginalnego obrazka
    skalowanie_konturow(kontury, 1.0/skala_do_wykrywania)
    # szukanie najwiekszego konturu na zdjeciu - bo prawdopodobnie bedzie nim plansza
    duze_kontury = znajdz_duze_kontury(kontury, plansza.shape, -1)
    maska = dobieranie_maski_dla_obrazka(plansza, duze_kontury, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    # dodanie kanału alfa do obrazka (przezroczystości)
    plansza_alfa = cv2.cvtColor(plansza, cv2.COLOR_BGR2BGRA)
    wyswietl_lub_zapisz(plansza_alfa, 'Krok 8 - Kanal przezroczystosci', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    # zamaskowanie planszy
    zamaskowana_plansza = cv2.bitwise_and(plansza_alfa, plansza_alfa, mask=maska)
    wyswietl_lub_zapisz(zamaskowana_plansza, 'Krok 9 - ' + 'Zamaskowana plansza', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    # wycięcie obszaru planszy
    przycieta_plansza = wycinanie_obrazka(zamaskowana_plansza, maska, wyswietlaj_kroki, zapisuj_kroki,
                                          sciezka_wyniki, oryginalny_obrazek=plansza)

    if wyswietlaj_kroki:
        cv2.waitKey(0)

    return przycieta_plansza


def obrobka_obrazka_przed_wykrywaniem_konturow_po_wybranym_kanale(obrazek_pomniejszony, nr_kanalu, wyswietlaj_kroki,
                                                                  zapisuj_kroki, sciezka_wyniki):
    kanaly = cv2.split(obrazek_pomniejszony)
    wyswietl_lub_zapisz(kanaly[nr_kanalu], 'Krok 1 - ' + 'Obrazek ' + str(nr_kanalu) + ' kanal', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    obrazek_wyrown_hist = cv2.equalizeHist(kanaly[nr_kanalu])
    wyswietl_lub_zapisz(obrazek_wyrown_hist, 'Krok 2 - ' + 'Obrazek wyrownany histogram', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    obrazek_rozmyty = cv2.medianBlur(obrazek_wyrown_hist, 15)  # było 15 dla małej planszy
    wyswietl_lub_zapisz(obrazek_rozmyty, 'Krok 3 - ' + 'Obrazek rozmyty', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    ret, obrazek_thresh = cv2.threshold(obrazek_rozmyty, 200, 255, cv2.THRESH_BINARY)
    wyswietl_lub_zapisz(obrazek_thresh, 'Krok 4 - ' + 'Obrazek thresh', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    return obrazek_thresh


def obrobka_obrazka_przed_wykrywaniem_konturow_po_kolorze(obrazek_pomniejszony, przedzial_kolorow,
                                                          wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    obrazek_wyrownany_hist = wyrownaj_histogram_obrazka(obrazek_pomniejszony)
    wyswietl_lub_zapisz(obrazek_wyrownany_hist, 'Krok 1 - ' + 'Obrazek z wyrownanym histogramem',
                        wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    # maska na piksele, które wpadają w dany przedział kolorów
    maska = cv2.inRange(obrazek_wyrownany_hist, przedzial_kolorow[0], przedzial_kolorow[1])
    wyswietl_lub_zapisz(maska, 'Krok 2 - '+'Maska po kolorze z przedzialu kolorow', wyswietlaj_kroki,
                        zapisuj_kroki, sciezka_wyniki)

    maska_rozmyta = cv2.medianBlur(maska, 5)
    wyswietl_lub_zapisz(maska_rozmyta, 'Krok 3 - ' + 'Maska rozmyta', wyswietlaj_kroki, zapisuj_kroki,
                        sciezka_wyniki)

    ret, maska_thresh = cv2.threshold(maska_rozmyta, 200, 255, cv2.THRESH_BINARY)
    wyswietl_lub_zapisz(maska_thresh, 'Krok 4 - ' + 'Maska thresh', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    return maska_thresh


def dobieranie_maski_dla_obrazka(obrazek, kontury, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    # 1-kanałowa maska, trzeba ją nałożyć na każdy kanał obrazka z osobna, żeby wyciąć dany obszar we wszystkich kolorach
    maska = np.zeros((obrazek.shape[0], obrazek.shape[1]), dtype='uint8')
    wyswietl_lub_zapisz(maska, 'Krok 6 - ' + 'Pusty', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    cv2.drawContours(maska, kontury, -1, (255, 255, 255), thickness=-1)
    wyswietl_lub_zapisz(maska, 'Krok 7 - ' + 'Maska', wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki)

    return maska


def wycinanie_obrazka(zamaskowany_obrazek, maska, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki, oryginalny_obrazek=None):
    # wycinanie planszy - znalezienie białych punktów i wybranie najbardziej wysuniętych punktów wykrytego kształtu
    y, x = np.where(maska == 255)
    (najmniejszy_y, najmniejszy_x) = (np.min(y), np.min(x))
    (najwiekszy_y, najwiekszy_x) = (np.max(y), np.max(x))

    if oryginalny_obrazek is not None:
        # test gdzie się wykryły punkty graniczne
        kopia_obrazka = oryginalny_obrazek.copy()
        cv2.rectangle(kopia_obrazka, (najmniejszy_x, najmniejszy_y), (najwiekszy_x, najwiekszy_y), (0, 255, 0), thickness=3)
        wyswietl_lub_zapisz(kopia_obrazka, 'Krok 10 - ' + 'Test przycinania', wyswietlaj_kroki, zapisuj_kroki,
                            sciezka_wyniki)

    # wycięcie planszy
    przyciety_obrazek = zamaskowany_obrazek[najmniejszy_y:najwiekszy_y, najmniejszy_x:najwiekszy_x]

    return przyciety_obrazek
