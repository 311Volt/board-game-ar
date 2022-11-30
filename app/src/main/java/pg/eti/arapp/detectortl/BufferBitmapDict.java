package pg.eti.arapp.detectortl;

import java.util.HashMap;

public class BufferBitmapDict {
    private HashMap<String, BufferBitmap> hashMap = new HashMap<>();

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
