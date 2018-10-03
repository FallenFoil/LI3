package parser;

import engine.DumpSO;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * 
 */
public class ParseTags extends DefaultHandler{

	private DumpSO dump;

	/**
     * Construtor que recebe a um DumpSO.
     *
     * @param dump 
     */
	public ParseTags(DumpSO dump){
		this.dump = dump;
	}	

    /**
     * Metodo chamado no inicio de cada elemento do ficheiro XML, para dar parse do ficheiro Tags.xml
     *
     * @param uri           Nao é usado
     * @param localName     Nao é usado
     * @param qName         Nome do elemento (ex. "row")
     * @param attributes    Atributo do elementos(ex. "Id")
     */
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {

        if (qName.equalsIgnoreCase("row")){
            long id = Long.parseLong(attributes.getValue("Id"));
            String tag = attributes.getValue("TagName");

            this.dump.addTag(id, tag);
        } 
    }
}
