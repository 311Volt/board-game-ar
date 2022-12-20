package pg.eti.arapp.ui;

import android.Manifest;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.media.Image;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.ArraySet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
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

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Optional;
import java.util.Set;

import pg.eti.arapp.R;
import pg.eti.arapp.catan.BoardInfo;
import pg.eti.arapp.catan.CatanBoardDetector;
import pg.eti.arapp.catan.CatanCardsDetector;
import pg.eti.arapp.catan.PlayerColor;
import pg.eti.arapp.catan.Settlement;
import pg.eti.arapp.catan.coord.VertexCoord;
import pg.eti.arapp.databinding.ActivityAractivityBinding;
import pg.eti.arapp.detectortl.BufferBitmap;
import pg.eti.arapp.catan.Player;
import pg.eti.arapp.utils.YuvConverter;

public class ARActivity extends AppCompatActivity {

    private ArFragment arFragment;
    private Renderable renderable;
    private ActivityAractivityBinding binding;

    private short step; // 0 - board, 1-3 - players cards
    private List<Player> players = new ArrayList<>();
    private static final String TAG = "Create file";

    private Set<TextView> scoreViews = new ArraySet<>();
    private int notification_id = 0;

    private boolean nextCardWasClicked = false;


//    Session session = null;
//    private SharedCamera sharedCamera;
//    private String cameraId;

    @Override
    @SuppressWarnings({"AndroidApiChecker", "FutureReturnValueIgnored"})
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityAractivityBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

//        createSession();
        
        requestRequiredPermissions();

        createNotificationChannel();


        //CatanCardsDetector cardsDetector = new CatanCardsDetector();
        //cardsDetector.initTesseract(getApplicationContext(), "pol");

        Toast.makeText(this, "Take photo using middle button, then click next", Toast.LENGTH_LONG).show();

        binding.takePhotoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try{
                    GetPicture();
                } catch (Exception x){
                    Log.e(TAG, "onClick: " + x.getMessage(), x);
                }
            }
        });



        binding.instructionsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (nextCardWasClicked) {
                    NextStepReady();
                    nextCardWasClicked = false;
                } else {
                    CatanBoardDetector detector = new CatanBoardDetector();
                    if (currentImage != null) {
                        YuvConverter yuvConverter = new YuvConverter(getApplicationContext(), currentImage.getWidth(), currentImage.getHeight());
                        Bitmap bmp = yuvConverter.toBitmap(currentImage);
//                    Bitmap bmp = bitmapAR;
                        currentImage.close();
                        currentImage = null;
                        if (bmp != null) {
                            if (!getIntent().getBooleanExtra("Experimental", false)) {
                                ProcessStep(detector, bmp);
                            } else {
                                // TODO: part for 3 players
                            }
                        }
                    }
                    else
                        NextStepReady();
                }
            }
        });

        binding.nextCardButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                nextCardWasClicked = true;
                CatanBoardDetector detector = new CatanBoardDetector();
                if (currentImage != null) {
                    YuvConverter yuvConverter = new YuvConverter(getApplicationContext(), currentImage.getWidth(), currentImage.getHeight());
                    Bitmap bmp = yuvConverter.toBitmap(currentImage);
//                    Bitmap bmp = bitmapAR;
                    currentImage.close();
                    currentImage = null;
                    if (bmp != null) {
                        if(!getIntent().getBooleanExtra("Experimental", false)) {
                            ProcessNextCard(bmp);
                        }
                        else {
                            // TODO: part for 3 players
                        }
                    }
                }
            }
        });

        arFragment = (ArFragment) getSupportFragmentManager().findFragmentById(R.id.ar_fragment);

        TextView textView = findViewById(R.id.instructions_text);
        textView.setText(R.string.phase_1);

        WeakReference<ARActivity> arActivityWeakReference = new WeakReference<>(this);

        ModelRenderable.builder()
                .setSource(this, Uri.parse("info_board.glb"))
                .setIsFilamentGltf(true)
                .build()
                .thenAccept(
                        modelRenderable -> {
                            ARActivity activity = arActivityWeakReference.get();
                            if (activity != null) {
                                activity.renderable = modelRenderable;
                            }
                        })
                .exceptionally(
                        throwable -> {
                            Toast toast =
                                    Toast.makeText(this, "Unable to load Info Board renderable", Toast.LENGTH_LONG);
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
                    model.setLocalScale(new Vector3(1.5f, 1.5f, 1.5f));
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

        initAnalyzer();

    }

//    public void createSession() {
//        // Create a new ARCore session.
//        try {
//            session = new Session(this, EnumSet.of(Session.Feature.SHARED_CAMERA));
//        } catch (UnavailableArcoreNotInstalledException e) {
//            e.printStackTrace();
//        } catch (UnavailableApkTooOldException e) {
//            e.printStackTrace();
//        } catch (UnavailableSdkTooOldException e) {
//            e.printStackTrace();
//        } catch (UnavailableDeviceNotCompatibleException e) {
//            e.printStackTrace();
//        }
//
//        // Store the ARCore shared camera reference.
//        sharedCamera = session.getSharedCamera();
//
//// Store the ID of the camera that ARCore uses.
//        cameraId = session.getCameraConfig().getCameraId();
//
//        // Create a session config.
//        Config config = new Config(session);
//
//        // Do feature-specific operations here, such as enabling depth or turning on
//        // support for Augmented Faces.
//
//        // Configure the session.
//        session.configure(config);
//    }

    private void initAnalyzer() {

        CatanBoardDetector.initializeDetector(getResources());
    }

//    private void NextCardReady(){
//        TextView view = findViewById(R.id.instructions_text);
//        switch(step){
//            case 1:
//                //view.setText(players.get(0).CameraText());
//                //view.setText("Jo, am artificial player 1");
//
//                view.setText(players.get(0).CameraText());
////                players.get(0).AddPoints((short) 10);
//                //players.get(0).AddScoreFromCards(2, true, true);
//
//                break;
//            case 2:
//                view.setText(players.get(1).CameraText());
//                break;
//            case 3:
//                view.setText(players.get(2).CameraText());
//                break;
//        }
//    }

    private void NextStepReady(){
        DisplayNotification();
        TextView view = findViewById(R.id.instructions_text);
        step = (short) ((step + 1) % (1 + players.size()));
        /*step++;
        if(step > 1)
            step = 0;*/

        switch(step){
            case 0:
                binding.nextCardButton.setVisibility(View.GONE);
                view.setText(R.string.phase_1);
                break;
            case 1:
                //view.setText(players.get(0).CameraText());
                //view.setText("Jo, am artificial player 1");
                binding.nextCardButton.setVisibility(View.VISIBLE);
                view.setText(players.get(0).CameraText());
//                players.get(0).AddPoints((short) 10);
                //players.get(0).AddScoreFromCards(2, true, true);

                break;
            case 2:
                binding.nextCardButton.setVisibility(View.VISIBLE);
                view.setText(players.get(1).CameraText());
                break;
            case 3:
                binding.nextCardButton.setVisibility(View.VISIBLE);
                view.setText(players.get(2).CameraText());
                break;
        }
    }

    private String CreateNotificationBoardText(){
        String result = "";
        for (Player player : players) {
            result += player.AnalysedBoard() + '\n';
        }
        return result;
    }

    private void ProcessNextCard(Bitmap bmp){
        switch(step){
            case 1:
                AnalyseCards(bmp, players.get(0));
                break;
            case 2:
                AnalyseCards(bmp, players.get(1));
                break;
            case 3:
                AnalyseCards(bmp, players.get(2));
                break;
        }
    }

    private void ProcessStep(CatanBoardDetector boardDetector, Bitmap bmp){
        switch(step){
            case 0:
                /*Log.d("Board: ", "We are here");
                players.clear();
                BoardInfo boardInfo = boardDetector.analyze(new BufferBitmap(bmp));
                for (HashMap.Entry<VertexCoord, Settlement> settlementEntry: boardInfo.settlements.entrySet()) {
                    Optional<Player> optionalPlayer = players.stream().filter(p -> p.getColor() == settlementEntry.getValue().playerColor).findAny();
                    Player player;
                    if(!optionalPlayer.isPresent()){
                        player = new Player(settlementEntry.getValue().playerColor);
                        players.add(player);
                    }else
                        player = optionalPlayer.get();
                    if (settlementEntry.getValue().isCity) {
                        player.AddCity();
                    } else {
                        player.AddSettlement();
                    }
                }*/
                Player player = new Player(PlayerColor.BLUE);
                players.add(player);
                player = new Player(PlayerColor.RED);
                players.add(player);
                player = new Player(PlayerColor.ORANGE);
                players.add(player);
                break;
            case 1:
                AnalyseCards(bmp, players.get(0));
                break;
            case 2:
                AnalyseCards(bmp, players.get(1));
                break;
            case 3:
                AnalyseCards(bmp, players.get(2));
                break;
        }
        Log.d("Step: ", this.step+"");
        NextStepReady();
        Log.d("Step after: ", this.step+"");
    }

    public void AnalyseCards(Bitmap bitmap, Player player){
        CatanCardsDetector cardsDetector = new CatanCardsDetector();
        cardsDetector.initTesseract(getApplicationContext(), "pol");

        ArrayList<BufferBitmap> cards = cardsDetector.getCardsNative(new BufferBitmap(bitmap));
        if(cards != null && !cards.isEmpty())
        {
            for(int i =0; i<cards.size(); i++) {
                Mat mat = cardsDetector.convertBitmapToMat(cards.get(i).toAndroidBitmap());
                int cardType = cardsDetector.recognizeCard(mat, true);
                Log.d("Cards", cardType + "");
                boolean longestRoad = false;
                if(cardType == 2)
                    player.AddScoreFromCards(0, true, false);
                else
                    if(cardType == 0)
                        Toast.makeText(this, "0 point card or not detected", Toast.LENGTH_SHORT).show();
                    else
                        player.AddScoreFromCards(cardType, false, false);

            }
        }
        cardsDetector.freeTesseract();
    }

    public void DisplayNotification(){
        NotificationCompat.Builder builder;

        String title = "";
        String text = "";

        switch(step){
            case 0:
                title = "Analysing the board complete";
                text = CreateNotificationBoardText();
                break;
            case 1:
                title = players.get(0).CompleteAnaysisText();
                text = players.get(0).AnalysedCards();
                break;
            case 2:
                title = players.get(1).CompleteAnaysisText();
                text = players.get(1).AnalysedCards();
                break;
            case 3:
                title = players.get(2).CompleteAnaysisText();
                text = players.get(2).AnalysedCards();
                break;
        }

        if(Build.VERSION.SDK_INT < 26)
            builder = new NotificationCompat.Builder(this, getResources().getString(R.string.channel_id))
                    .setSmallIcon(R.drawable.rounded_bg_foreground)
                    .setContentTitle(title)
                    .setContentText(text)
                    .setPriority(NotificationCompat.PRIORITY_DEFAULT);
        else
            builder = new NotificationCompat.Builder(this, getResources().getString(R.string.channel_id))
                    .setSmallIcon(R.drawable.rounded_bg_foreground)
                    .setContentTitle(title)
                    .setContentText(text)
                    .setStyle(new NotificationCompat.BigTextStyle()
                            .bigText(text))
                    .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        notificationManager.notify(notification_id, builder.build());
        notification_id++;
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= 26) {
            NotificationChannel channel = new NotificationChannel(
                    getResources().getString(R.string.channel_id),
                    getString(R.string.channel_name),
                    NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription(getString(R.string.channel_description));

            getSystemService(NotificationManager.class).createNotificationChannel(channel);
        }
    }

    private String ScoreText(){
        String result = "";
        for(Player p : players){
            result += p.toString() + "\n";
        }
        return result;
    }
    Image currentImage;
    public void NextStep(View view) {
        NextStepReady();
    }
    Bitmap bitmapAR;
    private void GetPicture(){
        try {
            if (currentImage != null)
                currentImage.close();
            currentImage = arFragment.getArSceneView().getArFrame().acquireCameraImage();

//            bitmapAR = Bitmap.createBitmap(1080, 1920, Bitmap.Config.ARGB_8888);
//            Handler handler = new Handler();
//            PixelCopy.request(arFragment.getArSceneView(), bitmapAR, (copyResult) -> {
//                if (copyResult == PixelCopy.SUCCESS) {
//                     //Save bitmap
//                } else {
//                     //Error
//                }
//            }, handler);

            String path = getPictureName();
            Toast.makeText(this, "Photo was taken", Toast.LENGTH_SHORT).show();
        } catch (NotYetAvailableException e) {
            e.printStackTrace();
            Toast.makeText(this, "Photo taking error ", Toast.LENGTH_SHORT).show();
        }
    }

    static private String[] requiredPermissions;
    static {
        ArrayList<String> tmp = new ArrayList<>(List.of(Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO));
        if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.P) {
            tmp.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
        requiredPermissions = new String[tmp.size()];
        requiredPermissions = tmp.toArray(requiredPermissions);
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