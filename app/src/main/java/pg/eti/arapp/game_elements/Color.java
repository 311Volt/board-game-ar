package pg.eti.arapp.game_elements;

public enum Color {
    BLUE ("Blue"),
    RED ("Red"),
    ORANGE ("Orange");

    private final String name;
    private final String smallName;

    Color(String name) {
        this.name = name;
        smallName = name.toLowerCase();
    }

    public String getName() {
        return name;
    }

    public String getSmallName() {
        return smallName;
    }
}
