package pg.eti.arapp.ui;

import android.net.Uri;
import android.os.Bundle;
import android.util.ArraySet;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.ar.core.Anchor;
import com.google.ar.core.HitResult;
import com.google.ar.core.Plane;
import com.google.ar.sceneform.AnchorNode;
import com.google.ar.sceneform.Node;
import com.google.ar.sceneform.math.Vector3;
import com.google.ar.sceneform.rendering.ModelRenderable;
import com.google.ar.sceneform.rendering.Renderable;
import com.google.ar.sceneform.rendering.ViewRenderable;
import com.google.ar.sceneform.ux.ArFragment;
import com.google.ar.sceneform.ux.TransformableNode;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import pg.eti.arapp.R;
import pg.eti.arapp.game_elements.Color;
import pg.eti.arapp.game_elements.Player;

public class ARActivity extends AppCompatActivity {

    private ArFragment arFragment;
    private Renderable renderable;

    private short step; // 0 - board, 1-3 - players cards
    private List<Player> players = new ArrayList<>();

    private Set<TextView> scoreViews = new ArraySet<>();
    @Override
    @SuppressWarnings({"AndroidApiChecker", "FutureReturnValueIgnored"})
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_aractivity);

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
}