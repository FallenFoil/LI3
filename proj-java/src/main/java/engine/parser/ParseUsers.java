package parser;

import engine.User;
import engine.DumpSO;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * 
 */
public class ParseUsers extends DefaultHandler{

    private DumpSO dump;

    /**
     * Construtor que recebe a um DumpSO.
     *
     * @param dump 
     */
    public ParseUsers(DumpSO dump){
        this.dump = dump;
    }

    /**
     * Metodo chamado no inicio de cada elemento do ficheiro XML, para dar parse do ficheiro Users.xml
     *
     * @param uri           Nao é usado
     * @param localName     Nao é usado
     * @param qName         Nome do elemento (ex. "row")
     * @param attributes    Atributo do elementos(ex. "Id")
     */
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {

        if (qName.equalsIgnoreCase("row")){
            long id = Long.parseLong(attributes.getValue("Id"));
            int rep = Integer.parseInt(attributes.getValue("Reputation"));
            String name = attributes.getValue("DisplayName");
            String bio = attributes.getValue("AboutMe");

            User user = new User(id, rep, name, bio);
            this.dump.addUser(id, user);
        }
    }
}
