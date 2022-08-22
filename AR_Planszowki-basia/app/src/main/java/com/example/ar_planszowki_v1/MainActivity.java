package com.example.ar_planszowki_v1;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.ContentUris;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Button;

import java.util.stream.Stream;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button wyswietlZdj = (Button) findViewById(R.id.przycisk_wyswietl_zdjecie);
        wyswietlZdj.setOnClickListener(new PrzyciskWyswietlZdjecie());

        Button wykonajZdj = (Button) findViewById(R.id.przycisk_wykonaj_zdjecie);
        wykonajZdj.setOnClickListener(new PrzyciskWykonajZdjecie());
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == PrzyciskWykonajZdjecie.KOD_WYKONAJ_ZDJECIE)
        {
            if (resultCode == RESULT_OK) {
                Log.d(PrzyciskWykonajZdjecie.TAG_WYKONAJ_ZDJECIE, "Udało się wykonać zdjęcie! :D Zapisało się pod scieżką: "
                        + (data != null ? data.getData() : "Brak ścieżki"));

                /*Cursor daneOPlikuNaZdj;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    daneOPlikuNaZdj = getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, null, null, null);
                    int numer = 0;
                    while(daneOPlikuNaZdj.moveToNext()) {
                        int indeks = daneOPlikuNaZdj.getColumnIndex(MediaStore.Images.Media.DISPLAY_NAME);
                        String nazwa = daneOPlikuNaZdj.getString(indeks);
                        Log.d(PrzyciskWykonajZdjecie.TAG_WYKONAJ_ZDJECIE, "Nazwa pliku w ContentProvider: " + nazwa);
                        indeks = daneOPlikuNaZdj.getColumnIndex(MediaStore.Images.Media._ID);
                        numer = daneOPlikuNaZdj.getInt(indeks);
                        Log.d(PrzyciskWykonajZdjecie.TAG_WYKONAJ_ZDJECIE, "Indeks pliku w ContentProvider: " + numer);
                    }
                }*/

            }
            else
                Log.d(PrzyciskWykonajZdjecie.TAG_WYKONAJ_ZDJECIE, "Nie udało się zapisać zdjęcia :<");
        }
    }
}