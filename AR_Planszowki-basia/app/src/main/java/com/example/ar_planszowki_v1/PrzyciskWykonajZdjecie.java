package com.example.ar_planszowki_v1;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.nfc.Tag;
import android.os.Build;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class PrzyciskWykonajZdjecie implements View.OnClickListener{

    static public int KOD_WYKONAJ_ZDJECIE = 10;
    static public String TAG_WYKONAJ_ZDJECIE = "WykonajZdjecie";

    @Override
    public void onClick(View view) {
        // 1. Przygotowanie pliku na zdjęcie dla aplikacji kamery, żeby miała gdzie zapisać dane

        // - kontekst aplikacji jest stabilniejszy, bo kontekst okna (aktywności) jest niszczony,
        // kiedy zamykamy te okno (albo trochę później, dokładniej kiedy to okno jest niszczone)
        Context kontekstAplikacji = view.getContext().getApplicationContext();
        // - ContentResolver jest do komunikacji z ContentProviderem - czymś, co udziela nam dostępu
        // do prywatnych plików innych aplikacji, lub czym my możemy udostępniać swoje prywatne
        // pliki innym aplikacjom
        ContentResolver resolver = kontekstAplikacji.getContentResolver();
        // - pobranie ścieżki do folderu współdzielonego zdjęć przez apki na urządzeniu
        Uri sciezkaDoPlikowZdjec;
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
            sciezkaDoPlikowZdjec = MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL_PRIMARY);
        else
            sciezkaDoPlikowZdjec = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        // - tak wyglądają szczegóły naszego "zamówienia" dla ContentProvider'a na
        // utworzenie dla nas pliku w pamięci, do której my sami nie mamy dostępu WRITE.
        // On stworzy plik w miejscu podanym przez URI i zapisze sobie, że zarządza teraz takim
        // plikiem w swojej tabeli z zarządzanymi plikami zdjęć
        ContentValues plikNaZdjecie = new ContentValues();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            plikNaZdjecie.put(MediaStore.Images.Media.RELATIVE_PATH, "Pictures"+"/"+kontekstAplikacji.getString(R.string.app_name));
        }
        else
        {
            //
        }
        plikNaZdjecie.put(MediaStore.Images.Media.DISPLAY_NAME, "Zdjecie15.jpg");
        // - zapisanie pustego pliku na zdjęcie - wysłanie "zamówienia" na plik do ContnetProvider'a
        // (w tym przypadku to MediaStore) używając ContentResolver'a
        Uri sciezkaDoZapisanegoZdj = resolver.insert(sciezkaDoPlikowZdjec, plikNaZdjecie);

        Log.d(TAG_WYKONAJ_ZDJECIE, "Udało się przygotować plik! "+sciezkaDoZapisanegoZdj);

        Cursor daneOPlikuNaZdj;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if(sciezkaDoZapisanegoZdj != null) {
                daneOPlikuNaZdj = resolver.query(sciezkaDoZapisanegoZdj, null, null, null);
                daneOPlikuNaZdj.moveToNext();
                int indeks = daneOPlikuNaZdj.getColumnIndex(MediaStore.Images.Media.DISPLAY_NAME);
                String nazwa = daneOPlikuNaZdj.getString(indeks);
                Log.d(TAG_WYKONAJ_ZDJECIE, "Nazwa pliku w ContentProvider: " + nazwa);
                indeks = daneOPlikuNaZdj.getColumnIndex(MediaStore.Images.Media._ID);
                int numer = daneOPlikuNaZdj.getInt(indeks);
                Log.d(TAG_WYKONAJ_ZDJECIE, "Indeks pliku w ContentProvider: " + numer);
            }
        }

        // 2. Dodanie danych do intencji uruchamiającej kamerę, w tym uri do pliku,
        // w którym ma zapisać dane zdjęcia
        Intent intencjaZrobieniaZdjecia = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        intencjaZrobieniaZdjecia.putExtra(MediaStore.EXTRA_OUTPUT, sciezkaDoZapisanegoZdj);
        // tylko klasa Activity ma metodę startActivityForResult (przestarzałą), nie mogłam do niej
        // dlatego użyć kontekstu aplikacji (to może być do poprawienia)
        AppCompatActivity okno = (AppCompatActivity) view.getContext();
        okno.startActivityForResult(intencjaZrobieniaZdjecia, KOD_WYKONAJ_ZDJECIE);
    }
}