package pg.eti.arapp.catan.coord;

import java.util.Objects;

public class CellCoord {
    public int x;
    public int y;
    public int z;

    public CellCoord(int x, int y, int z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public static CellCoord parse(String values) {
        String[] split = values.split(" ");
        return new CellCoord(
            Integer.parseInt(split[0]),
            Integer.parseInt(split[1]),
            Integer.parseInt(split[2])
        );
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        CellCoord cellCoord = (CellCoord) o;
        return x == cellCoord.x && y == cellCoord.y && z == cellCoord.z;
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y, z);
    }
}
