/*
 * Copyright 2008 Novamente LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package relex.parser_old;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Properties;

public class RelexProperties {

    public static int verbosity = 1;
    private static Properties props;

    public static Properties getProps() {
        return props;
    }

    public static String getProperty(String prop) {
        return props.getProperty(prop);
    }

    public static void setProperty(String prop, String val) {
        props.setProperty(prop, val);
    }

    public static int getIntProperty(String prop) {
        return Integer.parseInt(props.getProperty(prop));
    }

    private static void setDefaultValues() {
        // The number of parses before the socket client sends a kill signal to the server.
        // This protects from unexpected server errors due to out-of-memory from the linkparser
        setProperty("relex.parser.LinkParserSocketClient.parseCountBetweenKills", "0");
        setProperty(
                "relex.parser.LinkParserSocketClient.millisecondsBetweenConnectionAttempts",
                "5000");
        setProperty("relex.parser.LinkParserSocketClient.failedRequestRepeatLimit","3");

        // Allows the user change the dictionaries path using a System property, 
        // defined with -Drelex.parser.LinkParser.pathname=...
        // This is used in the Windows build. If undefined, uses default. 
		String pathname = System.getProperty("relex.parser.LinkParser.pathname");
		if (pathname!=null) setProperty("relex.parser.LinkParser.pathname", pathname);
    }
    
    private static void loadPropertiesIfRequired() {
        String loadFilename = System.getProperty("relex.RelexProperties.loadFilename");
        if (loadFilename != null && loadFilename.length() > 0) {
            if (verbosity >=1)
                System.err.println("RelexProperties: loading properties from " + loadFilename);
            try
            {
                props.load(new FileInputStream(new File(loadFilename)));
                if (verbosity >=1 )
                    System.err.println(RelexProperties.toPropsString());
            }
             catch (Exception e)
            {
                System.err.println("Property load failed.");
                e.printStackTrace();
            }
        }
    }
    
    private static void storePropertiesIfRequired() {
        String storeFilename = System.getProperty("relex.RelexProperties.storeFilename");
        if (storeFilename != null && storeFilename.length() > 0) {
            if (verbosity >=1)
                System.err.println("RelexProperties: storing properties in " + storeFilename);
            try {
                props.store(new FileOutputStream(new File(storeFilename)),"RelexProperties");
            } catch (Exception e)
            {
                e.printStackTrace();
            }
        }
    }
    
    public static String toPropsString() {
       return props.toString(); 
    }

    static {
        props = new Properties();
        setDefaultValues();
        loadPropertiesIfRequired();
        storePropertiesIfRequired();
    }

}
