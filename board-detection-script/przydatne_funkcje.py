import os


def wczytaj_sciezki_do_plikow(sciezka_glowna, lista_sciezek):
    lista_folderow_i_plikow = os.listdir(sciezka_glowna)
    for element in lista_folderow_i_plikow:
        if os.path.isfile(sciezka_glowna+'\\'+element):
            lista_sciezek.append(sciezka_glowna+'\\'+element)
        else:
            wczytaj_sciezki_do_plikow(sciezka_glowna+'\\'+element, lista_sciezek)
