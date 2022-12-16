import numpy as np
import cv2
from rembg import remove
import os


def wczytaj_sciezki_do_plikow(sciezka_glowna, lista_sciezek):
    lista_folderow_i_plikow = os.listdir(sciezka_glowna)
    for element in lista_folderow_i_plikow:
        if os.path.isfile(sciezka_glowna+'/'+element):
            lista_sciezek.append(sciezka_glowna+'/'+element)
        else:
            wczytaj_sciezki_do_plikow(sciezka_glowna+'/'+element, lista_sciezek)


def wycinanie_obrazka(zamaskowany_obrazek, maska):
    # wycinanie planszy - znalezienie białych punktów i wybranie najbardziej wysuniętych punktów wykrytego kształtu
    y, x = np.where(maska == 255)
    (najmniejszy_y, najmniejszy_x) = (np.min(y), np.min(x))
    (najwiekszy_y, najwiekszy_x) = (np.max(y), np.max(x))

    # wycięcie planszy
    przyciety_obrazek = zamaskowany_obrazek[najmniejszy_y:najwiekszy_y, najmniejszy_x:najwiekszy_x]

    return przyciety_obrazek


def wykryj_plansze(plansza, sciezka, nazwa):
    zamaskowana_plansza = remove(plansza)
    maska = zamaskowana_plansza[:, :, 3]
    #cv2.imwrite(sciezka+'\\'+nazwa+'.jpg', maska)
    przycieta_plansza = wycinanie_obrazka(zamaskowana_plansza, maska)
    return przycieta_plansza


sciezka_obrazki_wejsciowe = 'Zasoby/Osadnicy z Catanu/Zdjecia elementow gry/'
sciezka_obrazki_wynikowe = 'Wyniki/Wynikowe obrazki/Plansza'

# przygotowanie folderu na obrazki wynikowe
os.makedirs(sciezka_obrazki_wynikowe, exist_ok=True)

# wczytanie sciezek do obrazkow do przetworzenia
sciezki = []
#wczytaj_sciezki_do_plikow(sciezka_obrazki_wejsciowe+'/'+'Elementy/', sciezki)
wczytaj_sciezki_do_plikow(sciezka_obrazki_wejsciowe+'/'+'Plansza/', sciezki)
print(*sciezki, sep='\n')

for sciezka in sciezki:
    # przygotowanie folderu na wyniki
    nazwa_pliku = sciezka.split(os.sep)[-1]
    nazwa_pliku = nazwa_pliku[:-4]
    print(nazwa_pliku+'.png')

    # wykrycie i zapisanie wyciętej planszy
    plansza = cv2.imread(sciezka)
    przycieta_plansza = wykryj_plansze(plansza, sciezka_obrazki_wynikowe, nazwa_pliku)
    cv2.imwrite(sciezka_obrazki_wynikowe + '/' + nazwa_pliku + '.png', przycieta_plansza)
