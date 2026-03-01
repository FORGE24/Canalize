package cn.sanrol.canalize;

import net.minecraft.world.level.dimension.DimensionType;
import java.lang.reflect.Constructor;

public class TestDim {
    public static void main(String[] args) {
        for (Constructor<?> c : DimensionType.class.getConstructors()) {
            System.out.println("CTOR: " + c);
        }
    }
}
