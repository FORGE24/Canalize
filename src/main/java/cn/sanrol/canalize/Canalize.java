package cn.sanrol.canalize;

import org.slf4j.Logger;

import com.mojang.logging.LogUtils;
import com.mojang.serialization.MapCodec;

import net.minecraft.core.registries.Registries;
import net.neoforged.bus.api.IEventBus;
import net.neoforged.fml.common.Mod;
import net.neoforged.fml.config.ModConfig;
import net.neoforged.fml.ModContainer;
import net.neoforged.fml.event.lifecycle.FMLCommonSetupEvent;
import net.neoforged.neoforge.registries.DeferredHolder;
import net.neoforged.neoforge.registries.DeferredRegister;

import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.chunk.ChunkGenerator;
import cn.sanrol.canalize.world.NativeChunkGenerator;
import cn.sanrol.canalize.world.NativeBiomeSource;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

// The value here should match an entry in the META-INF/neoforge.mods.toml file
@Mod(Canalize.MODID)
public class Canalize {
    // Define mod id in a common place for everything to reference
    public static final String MODID = "canalize";
    // Directly reference a slf4j logger
    public static final Logger LOGGER = LogUtils.getLogger();
    
    // Config flags
    public static boolean CHUNK_LOAD_LOG  = false;
    public static boolean NATIVE_LOADED   = false;  // set true after successful loadNativeLibrary()
    public static boolean CHAT_DEBUG      = false;  // mirrors NativeLog::s_enabled

    // Create a Deferred Register to hold ChunkGenerators
    public static final DeferredRegister<MapCodec<? extends ChunkGenerator>> CHUNK_GENERATORS = DeferredRegister.create(Registries.CHUNK_GENERATOR, MODID);
    // Create a Deferred Register to hold BiomeSources
    public static final DeferredRegister<MapCodec<? extends BiomeSource>> BIOME_SOURCES = DeferredRegister.create(Registries.BIOME_SOURCE, MODID);

    public static final DeferredHolder<MapCodec<? extends ChunkGenerator>, MapCodec<NativeChunkGenerator>> NATIVE_CHUNK_GEN = CHUNK_GENERATORS.register("native_chunk_gen", () -> NativeChunkGenerator.CODEC);
    public static final DeferredHolder<MapCodec<? extends BiomeSource>, MapCodec<NativeBiomeSource>> NATIVE_BIOME_SOURCE = BIOME_SOURCES.register("native_biome_source", () -> NativeBiomeSource.CODEC);

    // The constructor for the mod class is the first code that is run when your mod is loaded.
    // FML will recognize some parameter types like IEventBus or ModContainer and pass them in automatically.
    public Canalize(IEventBus modEventBus, ModContainer modContainer) {
        // Setup plugins (extract from JAR to canalize_plugins folder)
        setupPlugins();

        // Load the native library
        loadNativeLibrary();

        // Register the commonSetup method for modloading
        modEventBus.addListener(this::commonSetup);

        // Register the Deferred Register to the mod event bus so chunk generators get registered
        CHUNK_GENERATORS.register(modEventBus);
        // Register the Deferred Register to the mod event bus so biome sources get registered
        BIOME_SOURCES.register(modEventBus);

        // Register our mod's ModConfigSpec so that FML can create and load the config file for us
        modContainer.registerConfig(ModConfig.Type.COMMON, Config.SPEC);
    }

    private void setupPlugins() {
        try {
            // 1. Create canalize_plugins directory in game root
            File pluginDir = new File("canalize_plugins");
            if (!pluginDir.exists()) {
                if (!pluginDir.mkdirs()) {
                    LOGGER.error("Failed to create plugin directory: " + pluginDir.getAbsolutePath());
                    return;
                }
            }

            // 2. Read plugins.list
            String listPath = "/natives/plugins.list";
            InputStream listStream = getClass().getResourceAsStream(listPath);
            if (listStream == null) {
                LOGGER.info("No built-in plugins list found at " + listPath);
                return;
            }

            List<String> plugins = new ArrayList<>();
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(listStream, StandardCharsets.UTF_8))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    line = line.trim();
                    if (!line.isEmpty()) {
                        plugins.add(line);
                    }
                }
            }

            // 3. Extract each plugin
            for (String pluginName : plugins) {
                String resourcePath = "/natives/plugins/" + pluginName;
                File targetFile = new File(pluginDir, pluginName);
                
                // For now, always overwrite to ensure version match
                try (InputStream is = getClass().getResourceAsStream(resourcePath)) {
                    if (is == null) {
                        LOGGER.warn("Plugin listed in plugins.list but not found in JAR: " + resourcePath);
                        continue;
                    }
                    
                    try (OutputStream os = new FileOutputStream(targetFile)) {
                        byte[] buffer = new byte[8192];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                    }
                    LOGGER.info("Extracted built-in plugin: " + pluginName);
                } catch (IOException e) {
                    LOGGER.error("Failed to extract plugin: " + pluginName, e);
                }
            }
            
        } catch (Exception e) {
            LOGGER.error("Error setting up plugins", e);
        }
    }

    private void loadNativeLibrary() {
        try {
            // Try standard loading first (e.g. dev environment)
            System.loadLibrary("canalize_native");
            initNative();
            NATIVE_LOADED = true;
            LOGGER.info("Loaded native library 'canalize_native' from java.library.path");
        } catch (UnsatisfiedLinkError e) {
            LOGGER.warn("Could not load native library from java.library.path. Attempting to extract from jar...");
            try {
                // Fallback: Extract from JAR
                // Note: The path inside the JAR depends on where build_native.bat put it.
                // We put it in src/main/resources/natives/canalize_native.dll
                String libName = "canalize_native.dll";
                String resourcePath = "/natives/" + libName;
                
                InputStream is = getClass().getResourceAsStream(resourcePath);
                if (is == null) {
                    LOGGER.error("Native library not found in jar at: {}", resourcePath);
                    throw new RuntimeException("Native library not found in jar: " + resourcePath);
                }

                File tempDir = new File(System.getProperty("java.io.tmpdir"), "canalize_natives");
                if (!tempDir.exists()) tempDir.mkdirs();
                File tempFile = new File(tempDir, libName);
                
                // Delete on exit, but we might want to cache it? 
                // For now, overwrite it every time to ensure latest version.
                try (OutputStream os = new FileOutputStream(tempFile)) {
                    byte[] buffer = new byte[8192];
                    int bytesRead;
                    while ((bytesRead = is.read(buffer)) != -1) {
                        os.write(buffer, 0, bytesRead);
                    }
                }
                
                System.load(tempFile.getAbsolutePath());
                initNative();
                NATIVE_LOADED = true;
                LOGGER.info("Loaded native library from extracted file: {}", tempFile.getAbsolutePath());

            } catch (Exception ex) {
                LOGGER.error("Failed to extract and load native library!", ex);
                // Don't crash the game — commands that query native will show 'not loaded'
            }
        }
    }

    // Native method declaration
    private static native void initNative();

    private void commonSetup(FMLCommonSetupEvent event) {
        LOGGER.info("[Canalize] Common setup complete.");
    }
}
