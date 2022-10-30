package pg.eti.arapp.detectortl;

import android.graphics.Bitmap;

import java.nio.Buffer;
import java.nio.ByteBuffer;

/**
 * A simplified bitmap type for communication with the native board detection module.
 * Underlying pixel format is always ARGB_8888.
 **/

public class BufferBitmap {
    public int width;
    public int height;
    public byte[] data;

    public BufferBitmap(Bitmap bitmap) {
        Bitmap rgb32 = bitmap.copy(Bitmap.Config.ARGB_8888, false);

        this.width = bitmap.getWidth();
        this.height = bitmap.getHeight();

        Buffer buffer = ByteBuffer.allocateDirect(bitmap.getAllocationByteCount());
        bitmap.copyPixelsToBuffer(buffer);
        data = (byte[])buffer.array();
    }

    public Bitmap toAndroidBitmap() {
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Buffer buffer = ByteBuffer.wrap(data);
        bitmap.copyPixelsFromBuffer(buffer);
        return bitmap;
    }

    public BufferBitmap(int width, int height, byte[] data) {
        this.width = width;
        this.height = height;
        this.data = data;
    }
}
