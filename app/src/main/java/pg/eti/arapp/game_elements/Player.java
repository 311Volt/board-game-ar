package pg.eti.arapp.game_elements;

public class Player {

    private Color color;
    private short score;
    private boolean longestRoad = false;
    private boolean highestKnight = false;
    private int pointsCards = 0;

    public Player(Color color, short score) {
        this.color = color;
        this.score = score;
    }

    public void AddScoreFromCards(int pointsCards, boolean longestRoad, boolean highestKnight){
        this.pointsCards = pointsCards;
        this.highestKnight = highestKnight;
        this.longestRoad = longestRoad;
        this.score += pointsCards;
        if(highestKnight)
            this.score += 2;
        if(longestRoad)
            this.score += 2;
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

    public String CompleteAnaysisText(){return String.format("Analysing cards of %s player complete", color.getSmallName());}

    public String AnalysedCards(){
        String knight = highestKnight ? "\nPlayer has Highest Knight card" : "";
        String road = longestRoad ? "\nPlayer has Longest Road card" : "";
        return String.format("Player has %d point cards", this.pointsCards) + knight + road;
    }
}
