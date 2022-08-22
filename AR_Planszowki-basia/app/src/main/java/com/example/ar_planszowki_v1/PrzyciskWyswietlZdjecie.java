package com.example.ar_planszowki_v1;

import android.content.Intent;
import android.view.View;


public class PrzyciskWyswietlZdjecie implements View.OnClickListener{
    @Override
    public void onClick(View view) {
        Intent intencjaPokazaniaZdj = new Intent(view.getContext(), WidokZdjecia.class);
        view.getContext().startActivity(intencjaPokazaniaZdj);
    }
}
