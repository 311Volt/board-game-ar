package pg.eti.arapp.catan;

import android.annotation.SuppressLint;

import pg.eti.arapp.catan.PlayerColor;

public class Player {

    private PlayerColor color;
    private short score;
    private boolean longestRoad = false;
    private boolean highestKnight = false;
    private int pointsCards = 0;
    private int settlements = 0;
    private int cities = 0;

    public Player(PlayerColor color) {
        this.color = color;
        this.score = 0;
        this.settlements = 0;
        this.cities = 0;
    }

    public void AddScoreFromCards(int pointsCards, boolean longestRoad, boolean highestKnight){
        this.pointsCards += pointsCards;
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
        String result = String.format("%s player: %d (%d)", color, score, getTotalNumCitiesAndSettlements());
        if(HasWon()) {
            result = "!!! " + result + " !!!";
        }
        return result;
    }

    public String CameraText(){
        return String.format("Place the camera over cards of %s player", color);
    }

    public String CompleteAnaysisText(){return String.format("Analysing cards of %s player complete", color);}

    public String AnalysedCards(){
        String knight = highestKnight ? "\nPlayer has Highest Knight card" : "";
        String road = longestRoad ? "\nPlayer has Longest Road card" : "";
        String score = "\nPlayer score: "+this.score;
        return String.format("Player has %d point cards", this.pointsCards) + knight + road + score;
    }

    public String AnalysedBoard(){
        return String.format("%s player has %d settlements and %d cities", this.color , this.settlements, this.cities);
    }

    public int getTotalNumCitiesAndSettlements() {
        return this.cities + this.settlements;
    }

    public void AddPoints(short points){
        this.score += points;
    }

    public void AddCity(){
        this.cities++;
        AddPoints((short) 2);
    }

    public void AddSettlement(){
        this.settlements++;
        AddPoints((short) 1);
    }

    public PlayerColor getColor() {
        return color;
    }
}
