package pg.eti.arapp.catan.coord;

import java.util.List;
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

    public List<VertexCoord> possibleNeighbors() {
        int mulHigh = high ? -1 : 1;
        return List.of(
            new VertexCoord(new CellCoord(origin.x, origin.y, origin.z), !high),
            new VertexCoord(new CellCoord(origin.x, origin.y - mulHigh, origin.z + mulHigh), !high),
            new VertexCoord(new CellCoord(origin.x + mulHigh, origin.y - mulHigh, origin.z), !high)
        );
    }
}
