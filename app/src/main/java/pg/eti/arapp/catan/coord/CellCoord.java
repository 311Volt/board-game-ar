package pg.eti.arapp.catan.coord;

import java.util.Objects;

public class CellCoord implements Comparable<CellCoord> {
    public int x;
    public int y;
    public int z;

    public CellCoord(int x, int y, int z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public static final CellCoord ORIGIN = new CellCoord(0,0,0);

    public static CellCoord parse(String values) {
        String[] split = values.split(" ");
        return new CellCoord(
            Integer.parseInt(split[0]),
            Integer.parseInt(split[1]),
            Integer.parseInt(split[2])
        );
   }

   public CellCoord diff(CellCoord other) {
        return new CellCoord(x-other.x, y-other.y, z-other.z);
   }

   public int distance(CellCoord other) {
        int dx = Math.abs(x - other.x);
        int dy = Math.abs(y - other.y);
        int dz = Math.abs(z - other.z);
        return Math.max(dx, Math.max(dy, dz));
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


    @Override
    public int compareTo(CellCoord rhs) {
        if(x != rhs.x) return Integer.compare(x, rhs.x);
        if(y != rhs.y) return Integer.compare(y, rhs.y);
        if(z != rhs.z) return Integer.compare(z, rhs.z);
        return 0;
    }
}
