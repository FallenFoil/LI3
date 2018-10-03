package parser;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;

import engine.DumpSO;
import engine.User;
import engine.Post;
import engine.Question;
import engine.Answer;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;


public class ParsePosts extends DefaultHandler{  

    private DumpSO dump;
    private Set<Answer> pendingPosts;


    /**
     * Construtor que recebe a um DumpSO.
     *
     * @param dump Onde se guarda a informação.
     */
    public ParsePosts(DumpSO dump){
        this.pendingPosts = new HashSet<>();
        this.dump = dump;
    }

    /**
     * Metodo que recebe uma String com as tags e separa-a em varias Strings, colocando-as num Set.
     *
     * @param tags Strings com as tags de um post.
     */
    private Set<String> parsePostTags(String tags){
        Set<String> set = new HashSet<String>();
        
        StringBuilder sb = new StringBuilder(tags);
        sb.deleteCharAt(0);
        sb.deleteCharAt(sb.length()-1);
        
        String[] parts = sb.toString().split("><");
        
        for(int i=0; i < parts.length; i++){
            set.add(parts[i]);
        }

        return set;
    }
    
    /**
     * Metodo que é chamado no final do parse dos posts, de forma a processar as respostas que ficaram pendentes.
     *
     * @param dump Onde se guarda a informação.
    */
    public void processPendingAns(DumpSO dump){
        for(Answer a : this.pendingPosts){
            Question q = (Question) dump.getPost(a.getQuestion());
            if(q != null){
                q.addAnswer(a.getId());
            }
        }
    }

    /**
     * Metodo chamado no inicio de cada elemento do ficheiro XML, para dar parse do ficheiro Posts.xml.
     *
     * @param uri           Nao é usado.
     * @param localName     Nao é usado.
     * @param qName         Nome do elemento (ex. "row").
     * @param attributes    Atributo do elementos(ex. "Id").
     */
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {

        if(qName.equalsIgnoreCase("row")){
            int postType = Integer.parseInt(attributes.getValue("PostTypeId"));
            if(postType != 1 && postType != 2) return;

            long id = Long.parseLong(attributes.getValue("Id"));
            LocalDate date = LocalDate.parse(attributes.getValue("CreationDate"), DateTimeFormatter.ISO_LOCAL_DATE_TIME);
            String ownerUserId = attributes.getValue("OwnerUserId");
            long owner = 0;
            
            if(ownerUserId!=null){
                owner = Long.parseLong(ownerUserId);
            }

            this.dump.addPostToUser(id, owner);
            if(postType == 1){ 
                //pergunta
                String title = attributes.getValue("Title");
                Set<String> tags = parsePostTags(attributes.getValue("Tags"));
                List<Long> ans = new ArrayList<>();

                Question q = new Question(title, tags, ans, id, date, owner);

                this.dump.addPost(id, q);
                this.dump.addQstByDate(q);
            }
            else{
                //resposta
                int score = Integer.parseInt(attributes.getValue("Score"));
                int comCount = Integer.parseInt(attributes.getValue("CommentCount"));
                long parent = Long.parseLong(attributes.getValue("ParentId"));

                Answer a = new Answer(score, comCount, parent, id, date, owner);
                    
                this.dump.addPost(id, a);
                this.dump.addAnsByDate(a);
                if (!this.dump.addAnsToQST(a)){
                    this.pendingPosts.add(a);
                }
            }
        } 
    }
}
