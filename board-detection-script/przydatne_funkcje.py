import os

import cv2
import numpy as np


def przeskaluj(obrazek, skala):
    if skala == 1.0:  # nie trzeba nic robić
        return obrazek
    nowa_szerokosc = int(obrazek.shape[1]*skala)
    nowa_wysokosc = int(obrazek.shape[0]*skala)

    if skala < 1.0:  # zmniejszanie
        return cv2.resize(obrazek, (nowa_szerokosc, nowa_wysokosc), interpolation=cv2.INTER_AREA)
    else:  # zwiększanie
        return cv2.resize(obrazek, (nowa_szerokosc, nowa_wysokosc), interpolation=cv2.INTER_CUBIC)


def wyswietl_lub_zapisz(element, nazwa, wyswietlaj_kroki, zapisuj_kroki, sciezka_wyniki):
    if wyswietlaj_kroki:
        cv2.imshow(nazwa, element)
    if zapisuj_kroki:
        cv2.imwrite(sciezka_wyniki+'\\'+nazwa+'.jpg', element)


def wczytaj_sciezki_do_plikow(sciezka_glowna, lista_sciezek):
    lista_folderow_i_plikow = os.listdir(sciezka_glowna)
    for element in lista_folderow_i_plikow:
        if os.path.isfile(sciezka_glowna+'\\'+element):
            lista_sciezek.append(sciezka_glowna+'\\'+element)
        else:
            wczytaj_sciezki_do_plikow(sciezka_glowna+'\\'+element, lista_sciezek)
