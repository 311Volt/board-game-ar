package butowska.anna.test2;

import static java.util.concurrent.TimeUnit.SECONDS;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.ArraySet;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.filament.gltfio.Animator;
import com.google.android.filament.gltfio.FilamentAsset;
import com.google.ar.core.Anchor;
import com.google.ar.core.HitResult;
import com.google.ar.core.Plane;
import com.google.ar.sceneform.AnchorNode;
import com.google.ar.sceneform.Node;
import com.google.ar.sceneform.math.Vector3;
import com.google.ar.sceneform.rendering.Color;
import com.google.ar.sceneform.rendering.Material;
import com.google.ar.sceneform.rendering.ModelRenderable;
import com.google.ar.sceneform.rendering.Renderable;
import com.google.ar.sceneform.rendering.ViewRenderable;
import com.google.ar.sceneform.ux.ArFragment;
import com.google.ar.sceneform.ux.TransformableNode;

import org.w3c.dom.Text;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.List;
import java.util.Set;

import butowska.anna.test2.databinding.FragmentLiveCameraBinding;

public class ARActivity extends AppCompatActivity {

//    private ArFragment arFragment;
//    private String MODEL_URL = "";
//
//    private static final double MIN_OPENGL_VERSION = 3.0;
//
//    private Renderable renderable;
//
//    private static class AnimationInstance {
//        Animator animator;
//        Long startTime;
//        float duration;
//        int index;
//
//        AnimationInstance(Animator animator, int index, Long startTime) {
//            this.animator = animator;
//            this.startTime = startTime;
//            this.duration = animator.getAnimationDuration(index);
//            this.index = index;
//        }
//    }
//
//    private final Set<AnimationInstance> animators = new ArraySet<>();
//
//    private final List<Color> colors =
//            Arrays.asList(
//                    new Color(0, 0, 0, 1),
//                    new Color(1, 0, 0, 1),
//                    new Color(0, 1, 0, 1),
//                    new Color(0, 0, 1, 1),
//                    new Color(1, 1, 0, 1),
//                    new Color(0, 1, 1, 1),
//                    new Color(1, 0, 1, 1),
//                    new Color(1, 1, 1, 1));
//    private int nextColor = 0;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_aractivity
//        );
//        arFragment = (ArFragment) getSupportFragmentManager().findFragmentById(R.id.ar_fragment1);
//
//        ModelRenderable.builder()
//                .setSource(
//                        this,
//                        Uri.parse(
//                                "model.glb"))
//                .setIsFilamentGltf(true)
//                .build()
//                .thenAccept(
//                        modelRenderable -> {
//                            renderable = modelRenderable;
//                        })
//                .exceptionally(
//                        throwable -> {
//                            Toast toast =
//                                    Toast.makeText(this, "Unable to load Tiger renderable", Toast.LENGTH_LONG);
//                            toast.setGravity(Gravity.CENTER, 0, 0);
//                            toast.show();
//                            return null;
//                        });
//
//        arFragment.setOnTapArPlaneListener(
//                (HitResult hitResult, Plane plane, MotionEvent motionEvent) -> {
//                    if (renderable == null) {
//                        return;
//                    }
//
//                    // Create the Anchor.
//                    Anchor anchor = hitResult.createAnchor();
//                    AnchorNode anchorNode = new AnchorNode(anchor);
//                    anchorNode.setParent(arFragment.getArSceneView().getScene());
//
//                    // Create the transformable model and add it to the anchor.
//                    TransformableNode model = new TransformableNode(arFragment.getTransformationSystem());
//                    model.setParent(anchorNode);
//                    model.setRenderable(renderable);
//                    model.select();
//
//                    FilamentAsset filamentAsset = model.getRenderableInstance().getFilamentAsset();
//                    if (filamentAsset.getAnimator().getAnimationCount() > 0) {
//                        animators.add(new AnimationInstance(filamentAsset.getAnimator(), 0, System.nanoTime()));
//                    }
//
//                    Color color = colors.get(nextColor);
//                    nextColor++;
//                    for (int i = 0; i < renderable.getSubmeshCount(); ++i) {
//                        Material material = renderable.getMaterial(i);
//                        material.setFloat4("baseColorFactor", color);
//                    }
//
//                    Node tigerTitleNode = new Node();
//                    tigerTitleNode.setParent(model);
//                    tigerTitleNode.setEnabled(false);
//                    tigerTitleNode.setLocalPosition(new Vector3(0.0f, 1.0f, 0.0f));
//                    ViewRenderable.builder()
//                            .setView(this, R.layout.tiger_card_view)
//                            .build()
//                            .thenAccept(
//                                    (renderable) -> {
//                                        tigerTitleNode.setRenderable(renderable);
//                                        tigerTitleNode.setEnabled(true);
//                                    })
//                            .exceptionally(
//                                    (throwable) -> {
//                                        throw new AssertionError("Could not load card view.", throwable);
//                                    }
//                            );
//                });
//
//        arFragment
//                .getArSceneView()
//                .getScene()
//                .addOnUpdateListener(
//                        frameTime -> {
//                            Long time = System.nanoTime();
//                            for (AnimationInstance animator : animators) {
//                                animator.animator.applyAnimation(
//                                        animator.index,
//                                        (float) ((time - animator.startTime) / (double) SECONDS.toNanos(1))
//                                                % animator.duration);
//                                animator.animator.updateBoneMatrices();
//                            }
//                        });
//
//    }


    private static final String TAG = ARActivity.class.getSimpleName();
    private static final double MIN_OPENGL_VERSION = 3.0;

    private ArFragment arFragment;
    private Renderable renderable;

    private static class AnimationInstance {
        Animator animator;
        Long startTime;
        float duration;
        int index;

        AnimationInstance(Animator animator, int index, Long startTime) {
            this.animator = animator;
            this.startTime = startTime;
            this.duration = animator.getAnimationDuration(index);
            this.index = index;
        }
    }

    private final Set<AnimationInstance> animators = new ArraySet<>();

    private final List<Color> colors =
            Arrays.asList(
                    new Color(0, 0, 0, 1),
                    new Color(1, 0, 0, 1),
                    new Color(0, 1, 0, 1),
                    new Color(0, 0, 1, 1),
                    new Color(1, 1, 0, 1),
                    new Color(0, 1, 1, 1),
                    new Color(1, 0, 1, 1),
                    new Color(1, 1, 1, 1));
    private int nextColor = 0;



    @Override
    @SuppressWarnings({"AndroidApiChecker", "FutureReturnValueIgnored"})
    // CompletableFuture requires api level 24
    // FutureReturnValueIgnored is not valid
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

//        if (!checkIsSupportedDeviceOrFinish(this)) {
//            return;
//        }

//        setContentView(R.layout.activity_aractivity);
        setContentView(R.layout.tiger_card_view);
        TextView myAwesomeTextView = findViewById(R.id.tiger_text);
        myAwesomeTextView.setText("T");
        setContentView(R.layout.activity_aractivity);
        arFragment = (ArFragment) getSupportFragmentManager().findFragmentById(R.id.ux_fragment);

        WeakReference<ARActivity> weakActivity = new WeakReference<>(this);

        ModelRenderable.builder()
                .setSource(
                        this,
                        Uri.parse(
                                "info_board.glb"))
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

                    // Create the Anchor.
                    Anchor anchor = hitResult.createAnchor();
                    AnchorNode anchorNode = new AnchorNode(anchor);
                    anchorNode.setParent(arFragment.getArSceneView().getScene());

                    // Create the transformable model and add it to the anchor.
                    TransformableNode model = new TransformableNode(arFragment.getTransformationSystem());
                    model.setParent(anchorNode);
                    model.setRenderable(renderable);
                    model.getScaleController().setMinScale(2.0f);
                    model.getScaleController().setMaxScale(3.0f);
                    model.setLocalScale(new Vector3(2.5f, 2.5f, 2.5f));
                    model.select();

                    FilamentAsset filamentAsset = model.getRenderableInstance().getFilamentAsset();
                    if (filamentAsset.getAnimator().getAnimationCount() > 0) {
                        animators.add(new AnimationInstance(filamentAsset.getAnimator(), 0, System.nanoTime()));
                    }

                    Color color = colors.get(nextColor);
                    nextColor++;
                    for (int i = 0; i < renderable.getSubmeshCount(); ++i) {
                        Material material = renderable.getMaterial(i);
                        material.setFloat4("baseColorFactor", color);
                    }

                    Node tigerTitleNode = new Node();
                    tigerTitleNode.setParent(model);
                    tigerTitleNode.setEnabled(false);
                    tigerTitleNode.setLocalPosition(new Vector3(0.0f, 0.0f, 0.05f));

//                    int k = R.id.tiger_text1;
//                    TextView myAwesomeTextView = findViewById(R.id.tiger_text1);
//                    myAwesomeTextView.setText("Udalo sie");
//                    setContentView(R.layout.tiger_card_view);
//                    setContentView(R.layout.activity_aractivity);
//                    setContentView(R.layout.tiger_card_view);
//                    TextView xd = findViewById(R.id.tiger_text);
//                    xd.setText("T");

                    TextView view = (TextView) getLayoutInflater().inflate(R.layout.tiger_card_view, null);
                    view.setText("T");


                    ViewRenderable.builder()
                            .setView(this, view)
                            .build()
                            .thenAccept(
                                    (renderable) -> {

                                        tigerTitleNode.setRenderable(renderable);
                                        tigerTitleNode.setEnabled(true);
                                    })
                            .exceptionally(
                                    (throwable) -> {
                                        throw new AssertionError("Could not load card view.", throwable);
                                    }
                            );
//                    setContentView(R.layout.activity_aractivity);

                });

        arFragment
                .getArSceneView()
                .getScene()
                .addOnUpdateListener(
                        frameTime -> {
                            Long time = System.nanoTime();
                            for (AnimationInstance animator : animators) {
                                animator.animator.applyAnimation(
                                        animator.index,
                                        (float) ((time - animator.startTime) / (double) SECONDS.toNanos(1))
                                                % animator.duration);
                                animator.animator.updateBoneMatrices();
                            }
                        });
    }

    private void ChangeWriting(){
        setContentView(R.layout.tiger_card_view);
        TextView myAwesomeTextView = findViewById(R.id.tiger_text);
        myAwesomeTextView.setText("T");
        setContentView(R.layout.activity_aractivity);
    }

    /**
     * Returns false and displays an error message if Sceneform can not run, true if Sceneform can run
     * on this device.
     *
     * <p>Sceneform requires Android N on the device as well as OpenGL 3.0 capabilities.
     *
     * <p>Finishes the activity if Sceneform can not run
     */
    public static boolean checkIsSupportedDeviceOrFinish(final Activity activity) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            Log.e(TAG, "Sceneform requires Android N or later");
            Toast.makeText(activity, "Sceneform requires Android N or later", Toast.LENGTH_LONG).show();
            activity.finish();
            return false;
        }
        String openGlVersionString =
                ((ActivityManager) activity.getSystemService(Context.ACTIVITY_SERVICE))
                        .getDeviceConfigurationInfo()
                        .getGlEsVersion();
        if (Double.parseDouble(openGlVersionString) < MIN_OPENGL_VERSION) {
            Log.e(TAG, "Sceneform requires OpenGL ES 3.0 later");
            Toast.makeText(activity, "Sceneform requires OpenGL ES 3.0 or later", Toast.LENGTH_LONG)
                    .show();
            activity.finish();
            return false;
        }
        return true;
    }

}