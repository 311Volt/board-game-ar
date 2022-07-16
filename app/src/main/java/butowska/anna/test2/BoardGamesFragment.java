package butowska.anna.test2;

import android.app.Activity;
import android.app.Instrumentation;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.fragment.NavHostFragment;

import butowska.anna.test2.databinding.FragmentBoardGamesBinding;
import butowska.anna.test2.databinding.FragmentPhotoBinding;

public class BoardGamesFragment extends Fragment {

    private FragmentBoardGamesBinding binding;
    private FragmentPhotoBinding photoBinding;

    private PhotoViewModel photoViewModel;

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {

        binding = FragmentBoardGamesBinding.inflate(inflater, container, false);
        photoBinding = FragmentPhotoBinding.inflate(inflater, container, false);
        return binding.getRoot();

    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.buttonMain.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                NavHostFragment.findNavController(BoardGamesFragment.this)
                        .navigate(R.id.action_SecondFragment_to_FirstFragment);
            }
        });

        binding.buttonChoosePhoto.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                startForResultFromGallery.launch(intent);
            }
        });
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    
    
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == 3 && data != null){
            Uri selectedImage = data.getData();
            //   ImageView imageView = findViewById(R.id.imageView);
//            imageView.setImageURI(selectedImage);
        }
    }

    private ActivityResultLauncher  startForResultFromGallery = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), new ActivityResultCallback<ActivityResult>() {
        @Override
        public void onActivityResult(ActivityResult result) {
            if (result.getResultCode() == Activity.RESULT_OK){
                try {
                    if (result.getData() != null){
                        Uri selectedImageUri = result.getData().getData();
                        photoViewModel = new ViewModelProvider(requireActivity()).get(PhotoViewModel.class);
                        photoViewModel.setImageUri(selectedImageUri);
                        //selectedImageUri.
                        //photoBinding.imageView3.setImageURI(selectedImageUri);
                        NavHostFragment.findNavController(BoardGamesFragment.this)
                                .navigate(R.id.action_SecondFragment_to_photoFragment);
                        int k = 0;
                    }
                }catch (Exception exception){
                    Log.d("TAG",""+exception.getLocalizedMessage());
                }
            }
        }
    });

    private ActivityResultLauncher  startForResultTakePhoto = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), new ActivityResultCallback<ActivityResult>() {
        @Override
        public void onActivityResult(ActivityResult result) {
            if (result.getResultCode() == Activity.RESULT_OK){
                try {
                    if (result.getData() != null){
                        Uri selectedImageUri = result.getData().getData();
                        // ImageView imageView = findViewById(R.id.imageView);
                        // imageView.setImageURI(selectedImage);
                    }
                }catch (Exception exception){
                    Log.d("TAG",""+exception.getLocalizedMessage());
                }
            }
        }
    });

}