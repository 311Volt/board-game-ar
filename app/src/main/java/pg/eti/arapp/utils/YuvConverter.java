package pg.eti.arapp.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.media.Image;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicYuvToRGB;
import android.renderscript.Type;

import java.nio.ByteBuffer;

public class YuvConverter {

    private final Allocation in, out;
    private final ScriptIntrinsicYuvToRGB script;

    public YuvConverter(Context context, int width, int height) {
        RenderScript rs = RenderScript.create(context);
        this.script = ScriptIntrinsicYuvToRGB.create(
                rs, Element.U8_4(rs));

        int yuvByteArrayLength = (int) (width * height * 1.5f);
        Type.Builder yuvType = new Type.Builder(rs, Element.U8(rs))
                .setX(yuvByteArrayLength);
        this.in = Allocation.createTyped(
                rs, yuvType.create(), Allocation.USAGE_SCRIPT);

        Type.Builder rgbaType = new Type.Builder(rs, Element.RGBA_8888(rs))
                .setX(width)
                .setY(height);
        this.out = Allocation.createTyped(
                rs, rgbaType.create(), Allocation.USAGE_SCRIPT);
    }

    public Bitmap toBitmap(Image image) {
        if (image.getFormat() != ImageFormat.YUV_420_888) {
            throw new IllegalArgumentException("Only supports YUV_420_888.");
        }

        byte[] yuvByteArray = toNv21(image);
        if (yuvByteArray == null) {
            return null;
        }
        in.copyFrom(yuvByteArray);
        script.setInput(in);
        script.forEach(out);

        Bitmap bitmap = Bitmap.createBitmap(
                image.getWidth(), image.getHeight(), Bitmap.Config.ARGB_8888);
        out.copyTo(bitmap);

        return bitmap;
    }

    public static byte[] toNv21(Image image) {
        byte[] nv21 = new byte[(int) (image.getWidth() * image.getHeight() * 1.5f)];
        if (!yuv420_888toNv21(
                image.getWidth(),
                image.getHeight(),
                image.getPlanes()[0].getBuffer(),
                image.getPlanes()[1].getBuffer(),
                image.getPlanes()[2].getBuffer(),
                image.getPlanes()[0].getPixelStride(),
                image.getPlanes()[1].getPixelStride(),
                image.getPlanes()[0].getRowStride(),
                image.getPlanes()[1].getRowStride(),
                nv21)) {
            return null;
        }
        return nv21;
    }

    public static native boolean yuv420_888toNv21(
            int imageWidth,
            int imageHeight,
            ByteBuffer yByteBuffer,
            ByteBuffer uByteBuffer,
            ByteBuffer vByteBuffer,
            int yPixelStride,
            int uvPixelStride,
            int yRowStride,
            int uvRowStride,
            byte[] nv21Output);

}
