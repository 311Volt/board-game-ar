package pg.eti.arapp.ui;

import static org.opencv.core.CvType.CV_8UC1;
import static org.opencv.imgproc.Imgproc.cvtColor;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.Image;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.util.ArraySet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.google.ar.core.Anchor;
import com.google.ar.core.HitResult;
import com.google.ar.core.Plane;
import com.google.ar.core.exceptions.NotYetAvailableException;
import com.google.ar.sceneform.AnchorNode;
import com.google.ar.sceneform.Node;
import com.google.ar.sceneform.math.Vector3;
import com.google.ar.sceneform.rendering.ModelRenderable;
import com.google.ar.sceneform.rendering.Renderable;
import com.google.ar.sceneform.rendering.ViewRenderable;
import com.google.ar.sceneform.ux.ArFragment;
import com.google.ar.sceneform.ux.TransformableNode;

import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.Set;

import pg.eti.arapp.R;
import pg.eti.arapp.databinding.ActivityAractivityBinding;
import pg.eti.arapp.game_elements.Color;
import pg.eti.arapp.game_elements.Player;

public class ARActivity extends AppCompatActivity {

    private ArFragment arFragment;
    private Renderable renderable;
    private ActivityAractivityBinding binding;
    private final Handler mHideHandler = new Handler(Looper.myLooper());
    private View mContentView;
    private View mControlsView;
    private boolean mVisible;

    private static final int UI_ANIMATION_DELAY = 300;
    private static final int AUTO_HIDE_DELAY_MILLIS = 3000;
    private static final boolean AUTO_HIDE = true;

    private short step; // 0 - board, 1-3 - players cards
    private List<Player> players = new ArrayList<>();
    private static final String TAG = "Create file";

    private Set<TextView> scoreViews = new ArraySet<>();



    @Override
    @SuppressWarnings({"AndroidApiChecker", "FutureReturnValueIgnored"})
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_aractivity);

        binding = ActivityAractivityBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        requestRequiredPermissions();

        binding.takePhotoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //goBack(view);
                try{
                    //Toast.makeText(getBaseContext(), "Trying...", Toast.LENGTH_SHORT).show();
//                    takePhoto(view);
                    GetPicture();
                } catch (Exception x){
                    Log.e(TAG, "onClick: " + x.getMessage(), x);
                }
            }
        });

        arFragment = (ArFragment) getSupportFragmentManager().findFragmentById(R.id.ar_fragment);

        players.add(new Player(Color.ORANGE, (short) 10));
        players.add(new Player(Color.BLUE, (short) 7));

        TextView textView = findViewById(R.id.instructions_text);
        textView.setText(R.string.phase_1);

        WeakReference<ARActivity> weakActivity = new WeakReference<>(this);

        ModelRenderable.builder()
                .setSource(this, Uri.parse("info_board.glb"))
                .setIsFilamentGltf(true)
                .build()
                .thenAccept(
                        modelRenderable -> {
                            ARActivity activity = weakActivity.get();
                            if (activity != null) {
                                activity.renderable = modelRenderable;
                            }
                        })
                .exceptionally(
                        throwable -> {
                            Toast toast =
                                    Toast.makeText(this, "Unable to load Tiger renderable", Toast.LENGTH_LONG);
                            toast.setGravity(Gravity.CENTER, 0, 0);
                            toast.show();
                            return null;
                        });

        arFragment.setOnTapArPlaneListener(
                (HitResult hitResult, Plane plane, MotionEvent motionEvent) -> {
                    if (renderable == null) {
                        return;
                    }
                    Anchor anchor = hitResult.createAnchor();
                    AnchorNode anchorNode = new AnchorNode(anchor);
                    anchorNode.setParent(arFragment.getArSceneView().getScene());

                    // adding info board to the world
                    TransformableNode model = new TransformableNode(arFragment.getTransformationSystem());
                    model.setParent(anchorNode);
                    model.setRenderable(renderable);
                    model.getScaleController().setMinScale(2.0f);
                    model.getScaleController().setMaxScale(3.0f);
                    model.setLocalScale(new Vector3(2.5f, 2.5f, 2.5f));
                    model.select();

                    // adding score to display on the info board
                    Node scoreNode = new Node();
                    scoreNode.setParent(model);
                    scoreNode.setEnabled(false);
                    scoreNode.setLocalPosition(new Vector3(0.0f, 0.0f, 0.05f));

                    // changing text to display
                    TextView view = (TextView) getLayoutInflater().inflate(R.layout.score_display_view, null);
                    view.setText("T");
                    scoreViews.add(view);

                    ViewRenderable.builder()
                            .setView(this, view)
                            .build()
                            .thenAccept(
                                    (r) -> {
                                        scoreNode.setRenderable(r);
                                        scoreNode.setEnabled(true);
                                    })
                            .exceptionally(
                                    (throwable) -> {
                                        throw new AssertionError("Could not load score view.", throwable);
                                    }
                            );

                });

        arFragment
                .getArSceneView()
                .getScene()
                .addOnUpdateListener(
                        frameTime -> {
                            // updating displayed score every frame
                            for(TextView scoreView : scoreViews){
                                scoreView.setText(String.format("%s", ScoreText()));
                            }
                        });
    }

    private void NextStepReady(){
        TextView view = findViewById(R.id.instructions_text);
        step = (short) ((step + 1) % (1 + players.size()));
        switch(step){
            case 0:
                view.setText(R.string.phase_1);
                break;
            case 1:
                view.setText(players.get(0).CameraText());
                break;
            case 2:
                view.setText(players.get(1).CameraText());
                break;
            case 3:
                view.setText(players.get(2).CameraText());
                break;
        }
    }

    private String ScoreText(){
        String result = "";
        for(Player p : players){
            result += p.toString() + "\n";
        }
        return result;
    }

    public void NextStep(View view) {
        NextStepReady();
    }

    private void GetPicture(){
        try {
            Image currentImage = arFragment.getArSceneView().getArFrame().acquireCameraImage();
            int imageFormat = currentImage.getFormat();
            String path = getPictureName();

            WriteImageInformation(currentImage, path);
            if (imageFormat == ImageFormat.YUV_420_888) {
                Log.d("ImageFormat", "Image format is YUV_420_888");
            }
            currentImage.close();
        } catch (NotYetAvailableException e) {
            e.printStackTrace();
        }
    }

    private void SavePicture(Image image){

    }

    private static byte[] NV21toJPEG(byte[] nv21, int width, int height) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        YuvImage yuv = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
        yuv.compressToJpeg(new Rect(0, 0, width, height), 100, out);
        return out.toByteArray();
    }

    public void WriteImageInformation(Image image, String path) {
        byte[] data = null;
        data = NV21toJPEG(YUV_420_888toNV21(image),
                image.getWidth(), image.getHeight());
        ImageView imgViewer = (ImageView) findViewById(R.id.image);
        Bitmap bm = BitmapFactory.decodeByteArray(data, 0, data.length);
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);

        imgViewer.setMinimumHeight(dm.heightPixels);
        imgViewer.setMinimumWidth(dm.widthPixels);
        imgViewer.setImageBitmap(bm);
    }

    private static byte[] YUV_420_888toNV21(Image image) {
        byte[] nv21;
        ByteBuffer yBuffer = image.getPlanes()[0].getBuffer();
        ByteBuffer uBuffer = image.getPlanes()[1].getBuffer();
        ByteBuffer vBuffer = image.getPlanes()[2].getBuffer();

        int ySize = yBuffer.remaining();
        int uSize = uBuffer.remaining();
        int vSize = vBuffer.remaining();

        nv21 = new byte[ySize + uSize + vSize];

        //U and V are swapped
        yBuffer.get(nv21, 0, ySize);
        vBuffer.get(nv21, ySize, vSize);
        uBuffer.get(nv21, ySize + vSize, uSize);

//        Mat mRgb = getYUV2Mat(image, nv21);

        return nv21;
    }

//    public static Mat getYUV2Mat(Image image, byte[] data) {
//        Mat mYuv = new Mat(image.getHeight() + image.getHeight() / 2, image.getWidth(), CV_8UC1);
//        mYuv.put(0, 0, data);
//        Mat mRGB = new Mat();
//        cvtColor(mYuv, mRGB, Imgproc.COLOR_YUV2RGB_NV21, 3);
//        return mRGB;
//    }

    static private String[] requiredPermissions;
    static {
        ArrayList<String> tmp = new ArrayList<>(List.of(Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO));
        if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.P) {
            tmp.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
        requiredPermissions = new String[tmp.size()];
        requiredPermissions = tmp.toArray(requiredPermissions);
    }

    private File createFolderIfNotExist() {
        File file = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES) + "/" + getApplicationContext().getString(R.string.app_name));
        if (!file.exists()) {
            if (!file.mkdir()) {
                Log.d(TAG, "Folder Create -> Failure");
            } else {
                Log.d(TAG, "Folder Create -> Success");
            }
        }
        return file;
    }

    private String getPictureName() {
        var date = new SimpleDateFormat("yyyyMMddHHmmssSSS", Locale.US).format(System.currentTimeMillis());
        String appName = getApplicationContext().getString(R.string.app_name) + " ";
        return appName.replace(" ", "_") + date + ".jpg";
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(requestCode == PERMISSIONS_REQUEST_CODE) {
            if(checkRequiredPermissions()) {
//                startCamera();
            } else {
                Toast.makeText(this, "error: user did not grant permissions", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private final static int PERMISSIONS_REQUEST_CODE = 10;

    private void requestRequiredPermissions() {
        ActivityCompat.requestPermissions(this, requiredPermissions, PERMISSIONS_REQUEST_CODE);
    }

    private boolean checkRequiredPermissions() {
        return Arrays
                .stream(requiredPermissions)
                .allMatch((p) -> ContextCompat.checkSelfPermission(getBaseContext(), p) == PackageManager.PERMISSION_GRANTED);
    }
}