package pg.eti.arapp.catan;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.util.HashMap;
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
        return BoardInfo.parse(data);
    }


    public static void initializeDetector(Resources resources) {
        var map = new HashMap<String, Bitmap>();

        map.put("cell_desert", BitmapFactory.decodeResource(resources, R.drawable.cell_desert));
        map.put("cell_fields", BitmapFactory.decodeResource(resources, R.drawable.cell_fields));
        map.put("cell_forest", BitmapFactory.decodeResource(resources, R.drawable.cell_forest));
        map.put("cell_hills", BitmapFactory.decodeResource(resources, R.drawable.cell_hills));
        map.put("cell_mountains", BitmapFactory.decodeResource(resources, R.drawable.cell_mountains));
        map.put("cell_pasture", BitmapFactory.decodeResource(resources, R.drawable.cell_pasture));
        map.put("element_blue", BitmapFactory.decodeResource(resources, R.drawable.element_blue));
        map.put("element_orange", BitmapFactory.decodeResource(resources, R.drawable.element_orange));
        map.put("element_red", BitmapFactory.decodeResource(resources, R.drawable.element_red));
        map.put("road_attenuation_mask", BitmapFactory.decodeResource(resources, R.drawable.road_attenuation_mask));
        map.put("sea_attenuation_mask", BitmapFactory.decodeResource(resources, R.drawable.sea_attenuation_mask));

        initializeDetectorNative(new BufferBitmapDict(map));
    }

    private static native void initializeDetectorNative(BufferBitmapDict dict);
}
