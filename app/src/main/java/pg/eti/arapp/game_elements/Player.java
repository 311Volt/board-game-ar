package pg.eti.arapp.game_elements;

public class Player {

    private Color color;
    private short score;

    public Player(Color color, short score) {
        this.color = color;
        this.score = score;
    }

    public boolean HasWon(){
        return score >= 10;
    }

    @Override
    public String toString() {
        return color.getName() + " player:\t" + score;
    }

    public String CameraText(){
        return String.format("Place the camera over cards of %s player", color.getSmallName());
    }
}
