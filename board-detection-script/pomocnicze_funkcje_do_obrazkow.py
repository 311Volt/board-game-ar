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
        if len(element.shape) > 2:  # jeśli obrazek ma więcej niż jeden kanał koloru
            if element.shape[2] > 3:  # jeśli mamy kanał alfa
                cv2.imwrite(sciezka_wyniki+'\\'+nazwa+'.png', element)
                return
        cv2.imwrite(sciezka_wyniki+'\\'+nazwa+'.jpg', element)


def wyrownaj_histogram_obrazka(obrazek):
    kanaly = cv2.split(obrazek)
    wyrownane_kanaly = []
    for kanal in kanaly:
        wyrownane_kanaly.append(cv2.equalizeHist(kanal))

    return cv2.merge(wyrownane_kanaly)


# poprawić, żeby zwracało kopię przeskalowanych konturów, a nie zmieniało oryginalne
def skalowanie_konturow(kontury, skala):
    for kontur in kontury:
        kontur[:, :, 0] = kontur[:, :, 0] * skala  # skalowanie x
        kontur[:, :, 1] = kontur[:, :, 1] * skala  # skalowanie y


def znajdz_najwiekszy_kontur(kontury):
    powierzchnia_najw_kont = 0
    najwiekszy_kontur = kontury[0]
    for kontur in kontury:
        powierzchnia = cv2.contourArea(kontur)
        if powierzchnia > powierzchnia_najw_kont:
            najwiekszy_kontur = kontur
            powierzchnia_najw_kont = powierzchnia

    return najwiekszy_kontur


# min_zajecie_obrazka=-1 - znajdz tylko 1, najwiekszy kontur
def znajdz_duze_kontury(kontury, wymiary_obrazka, min_zajecie_obrazka):
    if min_zajecie_obrazka == -1:
        najw_kontur = znajdz_najwiekszy_kontur(kontury)
        return [najw_kontur]

    print('Wymiary obrazka: '+str(wymiary_obrazka))
    powierzchnia_obrazka = wymiary_obrazka[0]*wymiary_obrazka[1]
    print('Powierzchnia obrazka: '+str(powierzchnia_obrazka))
    duze_kontury = []
    for kontur in kontury:
        powierzchnia = cv2.contourArea(kontur)
        print('Powierzchnia konturu: '+str(powierzchnia))
        stosunek_do_pow_obrazka = (powierzchnia / powierzchnia_obrazka)
        procent = round((stosunek_do_pow_obrazka * 100), 2)
        print('Procent powierzchni obrazka: '+str(procent)+'%')
        if stosunek_do_pow_obrazka >= min_zajecie_obrazka:
            duze_kontury.append(kontur)

    return duze_kontury
