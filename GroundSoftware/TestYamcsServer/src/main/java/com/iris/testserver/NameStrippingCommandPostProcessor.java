package com.iris.testserver;

import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Map;

import org.yamcs.YConfiguration;
import org.yamcs.commanding.PreparedCommand;
import org.yamcs.tctm.GenericCommandPostprocessor;
import org.yamcs.utils.ByteArrayUtils;

public class NameStrippingCommandPostProcessor implements GenericCommandPostprocessor {

    protected List<String> validCommandNames;
    protected List<byte[]> validCommandNamesAsBytes;

    public GenericCommandPostprocessor(String yamcsInstance) {
        super(yamcsInstance);
        readValidCommandNames(yamcsInstance, null);
    }

    public GenericCommandPostprocessor(String yamcsInstance, YConfiguration config) {
        super(yamcsInstance, config);
        readValidCommandNames(yamcsInstance, config);
    }

    protected readValidCommandNames(String yamcsInstance, YConfiguration config) {
        Path validCommandNameFilePath;

        if (config != null && config.containsKey("validCommandNameFileAbsPath")) {
            validCommandNameFilePath = Paths.get(config.getString("incomingDir"));
        } else {
            Path parent = YamcsServer.getServer().getConfigDirectory();
            validCommandNameFilePath = parent.resolve("validCommandNameFile.txt");
        }
   
        this.validCommandNames = Files.readAllLines(validCommandNameFilePath, Charset.defaultCharset());
        this.validCommandNames = Arrays.sort(this.validCommandNames,
                                             Comparator.comparingInt(String::length).reversed());

        for (String name : this.validCommandNames) {
            this.validCommandNamesAsBytes.add(name.getBytes("UTF-8"));
        }
    }

    @Deprecated
    public GenericCommandPostprocessor(String yamcsInstance, Map<String, Object> config) {
        this(yamcsInstance, YConfiguration.wrap(config));
    }

    @Override
    public byte[] process(PreparedCommand pc) {
        byte[] binary = pc.getBinary();

        boolean isValid = false;
        byte[] trimmedBinary;

        for (byte[] validNameAsBytes : this.validCommandNamesAsBytes) {
            if (BinaryArrayUtils.startsWith(binary, validNameAsBytes)) {
                isValid = true;
                trimmedBinary = Arrays.copyOfRange(binary, validNameAsBytes.length, binary.length);
                break;
            }
        }

        if (isValid) {
            return trimmedBinary;
        } else {
            return null;
        }
    }
}