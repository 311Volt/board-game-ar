# Skrypt do wykrywania planszy (Python)
## Zmiany
### Zmiana 5.10_v1
- dodałam plik .gitignore - ustawiłam go tak, żeby ignorował pliki w folderach "Zasoby" i "Wyniki" (no i oczywiście wirtualne środowisko i pliki projektu PyCharm itd.) - do zasobów wrzuciłam u siebie folder "Osadnicy z Catanu" ze zdjęciami i to jedne luźne zdjęcie wyciętej płytki siana ("IMG_20220924_155728567~2.jpg"), czyli wszystkie zdjęcia, które wam wysyłałam wcześniej
- zmieniłam też ścieżki w main.py żeby się wczytywały i zapisywały obrazki w tych folderach

Więc jak chcecie, żeby teraz działało, to też wrzućcie te zdjęcia do takich folderków u siebie lokalnie ^^

### Zmiana 9.10_v2

Wykrywanie planszy:
- plansza jest wykrywana z kolorowego obrazka w RGB, a nie z samego kanału *Blue* jak poprzednio - szukane są piksele o kolorze, który wpada w przedział kolorów, które uznajemy za niebieski, a nie wszystkie piksele o dużej wartości składowej *Blue* - dzięki temu wykrywanie jest odporniejsze na różne kolory tła i nie wykrywają się np. dodatkowe jasne elementy z otoczenia planszy
- wykrywanie planszy dobrze działa dla zdjęć w oświetleniu dziennym, ale nadal nie działa, gdy:
    - plansza jest prześwietlona - światło się od niej odbija i rozjaśnia mocno jej kolor
	- lub gdy nie jest widoczna na zdjęciu cała plansza - wykryty kształt wtedy się dzieli na kilka rozłącznych fragmentów, a nie jeden wspólny kształt planszy - nie może się domknąć kształt całej sześciokątnej maski i wycina się tylko niebieski fragment planszy
- na razie wykrywanie planszy nie działa dla innego oświetlenia niż dzienne - kolor planszy za mocno wtedy się różni od przedziału kolorów, które uznajemy za kolor planszy (ale działa dla niektórych zdjęć zrobionych po ciemku z oświetleniem tylko od latarki w telefonie)

Inne nowe funkcje:
- dodane wczytywanie i przetwarzanie wielu zdjęć naraz (z podanego folderu) - żeby łatwiej było porównywać skuteczność algorytmu po jego zmianach i czy nie jest gorzej dla przypadków, w których już działało (czyli w sumie dodane testowanie XD)