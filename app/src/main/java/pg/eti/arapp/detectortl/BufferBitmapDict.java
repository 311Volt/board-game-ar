package pg.eti.arapp.detectortl;

import android.graphics.Bitmap;

import java.util.HashMap;
import java.util.Map;

public class BufferBitmapDict {
    private HashMap<String, BufferBitmap> hashMap = new HashMap<>();

    public BufferBitmapDict() {

    }

    public BufferBitmapDict(Map<String, Bitmap> sourceData) {
        for (var kvp: sourceData.entrySet()) {
            putBitmap(kvp.getKey(), new BufferBitmap(kvp.getValue()));
        }
    }

    public HashMap<String, BufferBitmap> getHashMap() {
        return hashMap;
    }

    public String[] getKeySet() {
        return hashMap.keySet().toArray(new String[0]);
    }

    public void putBitmap(String key, BufferBitmap bmp) {
        hashMap.put(key, bmp);
    }

    public BufferBitmap getBitmap(String key) {
        return hashMap.get(key);
    }
}
