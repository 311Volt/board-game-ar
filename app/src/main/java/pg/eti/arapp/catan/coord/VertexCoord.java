package pg.eti.arapp.catan.coord;

import java.util.Objects;

public class VertexCoord {
    public CellCoord origin;
    public boolean high;

    public VertexCoord(CellCoord origin, boolean high) {
        this.origin = origin;
        this.high = high;
    }

    public static VertexCoord parse(String values) {
        String[] split = values.split("/");
        return new VertexCoord(CellCoord.parse(split[0]), Boolean.parseBoolean(split[1]));
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        VertexCoord vertexCoord = (VertexCoord) o;
        return high == vertexCoord.high && origin.equals(vertexCoord.origin);
    }

    @Override
    public int hashCode() {
        return Objects.hash(origin, high);
    }
}
