package butowska.anna.test2;

import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;

import com.google.ar.core.Anchor;
import com.google.ar.sceneform.AnchorNode;
import com.google.ar.sceneform.rendering.ModelRenderable;
import com.google.ar.sceneform.rendering.Renderable;
import com.google.ar.sceneform.rendering.ViewRenderable;
import com.google.ar.sceneform.ux.ArFragment;
import com.google.ar.sceneform.ux.TransformableNode;

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

        arFragment = (ArFragment) getChildFragmentManager().findFragmentById(R.id.ar_fragment);
        arFragment.setOnTapPlaneGlbModel("info_board.glb");

        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.buttonMain3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                NavHostFragment.findNavController(LiveCameraFragment.this)
                        .navigate(R.id.action_liveCameraFragment_to_SecondFragment);
            }
        });
    }
}