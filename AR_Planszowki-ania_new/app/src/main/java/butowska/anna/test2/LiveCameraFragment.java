package butowska.anna.test2;

import static java.util.concurrent.TimeUnit.SECONDS;

import android.net.Uri;
import android.os.Bundle;
import android.util.ArraySet;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;

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

import java.util.Arrays;
import java.util.List;
import java.util.Set;

import butowska.anna.test2.databinding.FragmentLiveCameraBinding;


public class LiveCameraFragment extends Fragment {// implements FragmentOnAttachListener, BaseArFragment.OnSessionConfigurationListener, ArFragment.OnViewCreatedListener, BaseArFragment.OnTapArPlaneListener {

    private FragmentLiveCameraBinding binding;
    private ArFragment arFragment;
    private ModelRenderable modelRenderable;
    private Renderable model;
    private ViewRenderable viewRenderable;
    private String MODEL_URL = "";


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        binding = FragmentLiveCameraBinding.inflate(inflater, container, false);


//        getActivity().setContentView(R.layout.fragment_live_camera);
//        arFragment = (ArFragment) getActivity().getSupportFragmentManager().findFragmentById(R.id.ar_fragment);
//        arFragment.setOnTapPlaneGlbModel("info_board.glb");
//        Node tigerTitleNode = new Node();
//        tigerTitleNode.setParent(model);
//        tigerTitleNode.setEnabled(false);
//        tigerTitleNode.setLocalPosition(new Vector3(0.0f, 1.0f, 0.0f));
//        ViewRenderable.builder()
//                .setView(this, R.layout.tiger_card_view)
//                .build()
//                .thenAccept(
//                        (renderable) -> {
//                            tigerTitleNode.setRenderable(renderable);
//                            tigerTitleNode.setEnabled(true);
//                        })
//                .exceptionally(
//                        (throwable) -> {
//                            throw new AssertionError("Could not load card view.", throwable);
//                        }
//                );


//        WeakReference<GltfActivity> weakActivity = new WeakReference<>(this);

//        ModelRenderable.builder()
//                .setSource(
//                        getContext(),
//                        Uri.parse(
//                                "model.glb"))
//                .setIsFilamentGltf(true)
//                .build()
//                .thenAccept(
//                        modelRenderable -> {
//                                renderable = modelRenderable;
//                        })
//                .exceptionally(
//                        throwable -> {
//                            Toast toast =
//                                    Toast.makeText(getContext(), "Unable to load Tiger renderable", Toast.LENGTH_LONG);
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
//                            .setView(getContext(), R.layout.tiger_card_view)
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


        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);


    }
}