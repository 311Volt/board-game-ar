import os
import cv2
import numpy as np

from przydatne_funkcje import przeskaluj
from wykrywanie_elementow import wykryj_plansze
from przydatne_funkcje import wczytaj_sciezki_do_plikow


sciezka_obrazki_wejsciowe = 'Zasoby\Osadnicy z Catanu\Zdjecia elementow gry\\'
sciezka_obrazki_wynikowe = 'Wyniki\Wynikowe obrazki\\'

# przygotowanie folderu na obrazki wynikowe
os.makedirs(sciezka_obrazki_wynikowe, exist_ok=True)

# wczytanie i wyświetlenie planszy
sciezki = []
wczytaj_sciezki_do_plikow(sciezka_obrazki_wejsciowe+'\\'+'Plansza\\', sciezki)
print(*sciezki, sep='\n')

for sciezka in sciezki:
    #plansza = cv2.imread(sciezka_obrazki_wejsciowe + '\\' + 'Plansza\Pusta plansza\IMG_20220924_160607794.jpg')
    plansza = cv2.imread(sciezka)
    plansza_mniejsza = przeskaluj(plansza, 0.2)
    #cv2.imshow('Plansza', plansza_mniejsza)

    # przygotowanie folderu na wyniki
    sciezka_na_wynik = sciezka_obrazki_wynikowe + '\\' + sciezka[len(sciezka_obrazki_wejsciowe):]
    sciezka_na_wynik = sciezka_na_wynik[:-4]
    print('\n' + sciezka_na_wynik)
    os.makedirs(sciezka_na_wynik, exist_ok=True)

    # wykrycie i zapisanie wyciętej planszy
    przycieta_plansza = wykryj_plansze(plansza, skala_do_wykrywania=0.2, zapisuj_kroki=True, sciezka_wyniki=sciezka_na_wynik)
    cv2.imwrite(sciezka_na_wynik + '\\' + 'wykryta_plansza_jolXD.png', przycieta_plansza)  # jeśli chcemy przezroczystość, musi być .png

# # template matching przykład
# plytka_siano = cv2.imread('Zasoby\IMG_20220924_155728567~2.jpg')
# plytka_siano_mniejsza = przeskaluj(plytka_siano, 0.2)
# plytka_siano_mniejsza = cv2.cvtColor(plytka_siano_mniejsza, cv2.COLOR_BGR2BGRA)
# cv2.imshow('Plytka siano', plytka_siano_mniejsza)
#
# mapa_jasnosci_dopasowania = cv2.matchTemplate(plytka_siano_mniejsza,
#                                               przycieta_plansza, cv2.TM_CCOEFF_NORMED)
# mapa_mniejsza = przeskaluj(mapa_jasnosci_dopasowania, 0.2)
#
# cv2.imshow('Mapa jasnosci', mapa_mniejsza)
#
# thresh = 0.34
# y_lok, x_lok = np.where(mapa_jasnosci_dopasowania >= thresh)
# print(len(y_lok))
#
# kopia_planszy = przycieta_plansza.copy()
# szerokosc = int(plytka_siano_mniejsza.shape[1])
# wysokosc = int(plytka_siano_mniejsza.shape[0])
#
# for (x, y) in zip(x_lok, y_lok):
#     cv2.rectangle(kopia_planszy, (x, y), (x+szerokosc, y+wysokosc), (0, 255, 0, 255),
#                   thickness=2)
#
# kopia_planszy_mniejsza = przeskaluj(kopia_planszy, 0.2)
# cv2.imshow('Wykryte cosie', kopia_planszy_mniejsza)
# cv2.imwrite(sciezka_obrazki_wynikowe+'\\'+'Wykryte cosie.png', kopia_planszy)
#
# # kontury plytek
# plansza_szara = cv2.cvtColor(przycieta_plansza, cv2.COLOR_BGRA2GRAY)
# ret, plansza_thresh = cv2.threshold(plansza_szara, 125, 255, cv2.THRESH_BINARY)
# kontury, hierarchia = cv2.findContours(plansza_thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
# pusty_obrazek = np.zeros((przycieta_plansza.shape[0], przycieta_plansza.shape[1]), dtype='uint8')
# cv2.drawContours(pusty_obrazek, kontury, -1, 255, thickness=2)
# mniejszy_obrazek_kontury = przeskaluj(pusty_obrazek, 0.2)
# cv2.imshow('Kontury plytek', mniejszy_obrazek_kontury)
# cv2.imwrite(sciezka_obrazki_wynikowe+'\\'+'Kontury_plytek.jpg', pusty_obrazek)

cv2.waitKey(0)
