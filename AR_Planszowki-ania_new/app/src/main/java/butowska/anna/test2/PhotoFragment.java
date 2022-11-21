package butowska.anna.test2;

import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.fragment.NavHostFragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import butowska.anna.test2.databinding.FragmentPhotoBinding;

public class PhotoFragment extends Fragment {

    private FragmentPhotoBinding binding;

    private PhotoViewModel photoViewModel;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        binding = FragmentPhotoBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        Uri selectedImageUri;
        Bitmap bitmap;
        photoViewModel = new ViewModelProvider(requireActivity()).get(PhotoViewModel.class);
        selectedImageUri = photoViewModel.getImageUri();
        bitmap = photoViewModel.getImageBitmap();

        if(selectedImageUri != null)
            binding.imageView3.setImageURI(selectedImageUri);
        if(bitmap != null)
            binding.imageView3.setImageBitmap(bitmap);

        binding.buttonBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                NavHostFragment.findNavController(PhotoFragment.this)
                        .navigate(R.id.action_photoFragment_to_SecondFragment);
            }
        });
    }
}