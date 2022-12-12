package pg.eti.arapp.catan;

import java.util.ArrayList;

import pg.eti.arapp.detectortl.BufferBitmap;

public class CatanCardsDetector {

    public native ArrayList<BufferBitmap> getCardsNative(BufferBitmap bitmap);


}
