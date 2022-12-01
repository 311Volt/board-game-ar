package pg.eti.arapp.catan;

import android.content.res.Resources;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.util.Map;

import pg.eti.arapp.R;
import pg.eti.arapp.detectortl.BufferBitmap;
import pg.eti.arapp.detectortl.BufferBitmapDict;

public class CatanBoardDetector {

    public native BufferBitmap analyzeToImage(BufferBitmap bitmap);
    public native String analyzeNative(BufferBitmap bitmap);

    public BoardInfo analyze(BufferBitmap bitmap) {
        String data = analyzeNative(bitmap);
        Log.d("analyzer", data);
        return BoardInfo.parse(analyzeNative(bitmap));
    }


    public static void initializeDetector(Resources resources) {

        initializeDetectorNative(new BufferBitmapDict(Map.of(
            "cell_desert", BitmapFactory.decodeResource(resources, R.drawable.cell_desert),
            "cell_fields", BitmapFactory.decodeResource(resources, R.drawable.cell_fields),
            "cell_forest", BitmapFactory.decodeResource(resources, R.drawable.cell_forest),
            "cell_hills", BitmapFactory.decodeResource(resources, R.drawable.cell_hills),
            "cell_mountains", BitmapFactory.decodeResource(resources, R.drawable.cell_mountains),
            "cell_pasture", BitmapFactory.decodeResource(resources, R.drawable.cell_pasture),
            "element_blue", BitmapFactory.decodeResource(resources, R.drawable.element_blue),
            "element_orange", BitmapFactory.decodeResource(resources, R.drawable.element_orange),
            "element_red", BitmapFactory.decodeResource(resources, R.drawable.element_red)
        )));
    }

    private static native void initializeDetectorNative(BufferBitmapDict dict);
}
