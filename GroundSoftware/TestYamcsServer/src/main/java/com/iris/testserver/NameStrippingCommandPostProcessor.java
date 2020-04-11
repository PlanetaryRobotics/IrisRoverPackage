package com.iris.testserver;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;

import org.yamcs.ConfigurationException;
import org.yamcs.YamcsServer;
import org.yamcs.YConfiguration;
import org.yamcs.commanding.PreparedCommand;
import org.yamcs.tctm.GenericCommandPostprocessor;
import org.yamcs.utils.ByteArrayUtils;

public class NameStrippingCommandPostProcessor extends GenericCommandPostprocessor {

    protected List<String> validCommandNames;
    protected List<byte[]> validCommandNamesAsBytes;

    public NameStrippingCommandPostProcessor(String yamcsInstance) throws ConfigurationException {
        super(yamcsInstance);
        readValidCommandNames(yamcsInstance, null);
    }

    public NameStrippingCommandPostProcessor(String yamcsInstance, YConfiguration config) 
            throws ConfigurationException {
        super(yamcsInstance, config);
        readValidCommandNames(yamcsInstance, config);
    }

    protected void readValidCommandNames(String yamcsInstance, YConfiguration config) 
            throws ConfigurationException {
        try {
            Path validCommandNameFilePath;

            if (config != null && config.containsKey("validCommandNameFileAbsPath")) {
                validCommandNameFilePath = Paths.get(config.getString("incomingDir"));
            } else {
                Path parent = YamcsServer.getServer().getConfigDirectory();
                validCommandNameFilePath = parent.resolve("validCommandNameFile.txt");
            }
       
            this.validCommandNames = Files.readAllLines(validCommandNameFilePath, Charset.defaultCharset());
            Collections.sort(this.validCommandNames, Comparator.comparingInt(String::length).reversed());

            for (String name : this.validCommandNames) {
                this.validCommandNamesAsBytes.add(name.getBytes("UTF-8"));
            }
        } catch(IOException ex) {
            throw new ConfigurationException(ex);
        }
    }

    @Deprecated
    public NameStrippingCommandPostProcessor(String yamcsInstance, Map<String, Object> config) {
        this(yamcsInstance, YConfiguration.wrap(config));
    }

    @Override
    public byte[] process(PreparedCommand pc) {
        byte[] binary = pc.getBinary();

        for (byte[] validNameAsBytes : this.validCommandNamesAsBytes) {
            if (ByteArrayUtils.startsWith(binary, validNameAsBytes)) {
                return Arrays.copyOfRange(binary, validNameAsBytes.length, binary.length);
            }
        }

        return null;
    }
}