package engine;

import java.time.LocalDate;

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.HashSet;
import java.util.Iterator;
import java.util.stream.Collectors;

import java.io.File;
import java.io.IOException;

import li3.TADCommunity;

import common.MyLog;
import common.Pair;

import parser.ParseUsers;
import parser.ParsePosts;
import parser.ParseTags;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.SAXException;


public class TCDCommunity implements TADCommunity
{
    private DumpSO dump;


    public TCDCommunity() {
        init();
    }

    public void init(){
        this.dump = new DumpSO();
    }

    public void load(String dumpPath){
        
        SAXParserFactory saxParserFactory = SAXParserFactory.newInstance();
        try{
            SAXParser saxParser = saxParserFactory.newSAXParser();

            ParseUsers users = new ParseUsers(this.dump);
            ParsePosts posts = new ParsePosts(this.dump);
            ParseTags tags = new ParseTags(this.dump);

            System.out.println("Parsing users...");
            saxParser.parse(new File(dumpPath + "/Users.xml"), users);

            System.out.println("Parsing tags...");
            saxParser.parse(new File(dumpPath + "/Tags.xml"), tags);

            System.out.println("Parsing posts...");
            saxParser.parse(new File(dumpPath + "/Posts.xml"), posts);
            posts.processPendingAns(dump);

            System.out.println("Sorting users...");
            this.dump.addUsersByPosts();
            this.dump.addUsersByReputation();

            System.out.println("Dump complete.");
        }
        catch(ParserConfigurationException | SAXException | IOException e){
            e.printStackTrace();
        }
    }

    /**
     * Query 1
     * Recebe id post, devolve par com titulo e nome do autor (se for resposta, da pergunta correspondente).
     * Se nao encontrar devolve par de Strings vazias.
     *
     * @param  id   id do post
     *
     * @return      par com titulo e nome do autor do post, nao null
     */
    public Pair<String,String> infoFromPost(long id){

        Post post = dump.getPost(id);
        if (post ==  null) return new Pair<String,String>("","");

        if(post instanceof Answer){
            post = dump.getPost(((Answer) post).getQuestion());
        }

        String title = ((Question) post).getTitle();
        String author = dump.getUser(post.getOwnerId()).getDisplayName();

        return new Pair<String,String>(title, author);
    }

    /**
     * Query 2
     * Top N utilizadores com mais posts.
     * Pode retornar uma lista com menos de N elementos se não existirem tantos users.
     *
     * @param   N   numero de utilizadores a guardar na lista
     *
     * @return      lista de utilizadores ordenada por numero de posts, nao null
     */
    public List<Long> topMostActive(int N){

        List<Long> lst = new ArrayList<>(N);
        for(User usr : dump.getTopUsersByPosts(N)){
            lst.add(usr.getId());
        }

        return lst;
    }


    /**
     * Query 3
     * Recebe intervalo de tempo, devolve numero de perguntas e respostas.
     *
     * @param  begin    data inicial do intervalo
     * @param  end      data final do intervalo
     *
     * @return          par com numero de perguntas e respostas, nao null
     */
    public Pair<Long, Long> totalPosts(LocalDate begin, LocalDate end){

        Long qst = new Long(dump.getQuestionsByDate(begin, end).size());
        Long ans = new Long(dump.getAnswersByDate(begin, end).size());

        return new Pair<Long, Long>(qst, ans);
    }

    /**
     * Query 4
     * Recebe intervalo de tempo, devolve ids das perguntas com determinada tag (ordem cronologica inversa).
     *
     * @param  tag      tag
     * @param  begin    data inicial do intervalo
     * @param  end      data final do intervalo
     *
     * @return          lista de ids das perguntas
     */
    public List<Long> questionsWithTag(String tag, LocalDate begin, LocalDate end){

        List<Long> lst = new ArrayList<>();

        for(Question qst : dump.getQuestionsByDate(begin, end)){
            if(qst.getTags().contains(tag)){
                lst.add(qst.getId());
            }
        }

        return lst;                               
    }


    /**
     * Query 5
     * Recebe id de um utilizador, devolve aboutMe e ids dos ultimos 10 posts (ordem cronologica inversa).
     * Se nao encontrar devolve par com String e lista vazias.
     *
     * @param   id  id do utilizador
     *
     * @return      par com aboutMe e lista com ids dos ultimos posts, nao null
     */
    public Pair<String, List<Long>> getUserInfo(long id){

        User user = dump.getUser(id);
        ArrayList<Long> lst = new ArrayList<>(10);
        if (user ==  null) return new Pair<String,List<Long>>("",lst);

        String aboutMe = user.getAboutMe();
        List<Post> posts = dump.getPostsByDate(user.getPosts());

        for(int i = 0; i < posts.size() && i < 10; i++){
            lst.add(posts.get(i).getId());
        }

        return new Pair<String, List<Long>>(aboutMe,lst);
    }

    /**
     * Query 6
     * Recebe intervalo de tempo, devolve ids das N respostas com mais votos(score) por ordem decrescente.
     *
     * @param  N        numero de respostas na lista
     * @param  begin    data inicial do intervalo
     * @param  end      data final do intervalo
     *
     * @return          lista de ids das respostas, nao null
     */
    public List<Long> mostVotedAnswers(int N, LocalDate begin, LocalDate end){

        List<Answer> answers = dump.getAnswersByDate(begin, end);
        answers.sort(new Answer.compareScore());

        ArrayList<Long> lst = new ArrayList<>(N);
        
        for(int i = 0; i < answers.size() && i < N; i++){
            lst.add(answers.get(i).getId());
        }

        return lst;
    }

    /**
     * Query 7
     * Recebe intervalo de tempo, devolve ids das N perguntas com mais respostas por ordem decrescente do numero de respostas.
     * Tanto as perguntas como as respostas consideradas estao no intervalo dado.
     *
     * @param  N        numero de perguntas na lista
     * @param  begin    data inicial do intervalo
     * @param  end      data final do intervalo
     *
     * @return          lista de ids das perguntas, nao null
     */
    public List<Long> mostAnsweredQuestions(int N, LocalDate begin, LocalDate end){

        List<Question> questions = dump.getQuestionsByDate(begin, end);
        List<Pair<Question, Integer>> qstAndAns = new ArrayList<>();
        for(Question q : questions){
            int c = 0;
            for(Long ans : q.getAnswers()){
                LocalDate d = dump.getPost(ans).getCreationDate();
                if(!(d.isBefore(begin) || d.isAfter(end))) c++;
            }
            qstAndAns.add(new Pair(q, c));
        }
        qstAndAns.sort((a,b) -> Integer.compare(b.getSnd(), a.getSnd())); 
        
        ArrayList<Long> lst = new ArrayList<>(N);
        
        for(int i = 0; i < qstAndAns.size() && i < N; i++){
            lst.add(qstAndAns.get(i).getFst().getId());
        }

        return lst;
    }

    /**
     * Query 8
     * Recebe palavra, devolve lista de ids de N perguntas cujos titulos a contenham (ordem cronologica inversa).
     *
     * @param  N        numero de perguntas na lista
     * @param  word     palavra
     *
     * @return          lista de ids das perguntas, nao null
     */
    public List<Long> containsWord(int N, String word){

        List<Long> lst = new ArrayList<>(N);

        for(Question qst : dump.getQuestionsWithWord(N, word)){
            lst.add(qst.getId());
        }

        return lst;
    }

    /**
     * Query 9
     * Recebe ids de dois users, devolve lista de ids  das ultimas N perguntas em que os dois participaram (ordem cronologica inversa).
     *
     * @param   N       numero de perguntas na lista
     * @param   id1     id de um user
     * @param   id2     id de outro user
     *
     * @return          lista de ids das perguntas, nao null
     */
    public List<Long> bothParticipated(int N, long id1, long id2){

        List<Long> lst = new ArrayList<>();

        User user1 = dump.getUser(id1);
        User user2 = dump.getUser(id2);
        if((user1 == null) || (user2 == null)) return lst;

        List<Post> posts1 = dump.getPostsByDate(user1.getPosts());
        List<Post> posts2 = dump.getPostsByDate(user2.getPosts());
        if(posts1.isEmpty() || posts2.isEmpty()) return lst;

        List<Post> postsA;
        List<Post> postsB;
        if(posts1.size() < posts2.size()){
            postsA = posts1;
            postsB = posts2;
        } else {
            postsA = posts2;
            postsB = posts1;
        }

        //Inicializa um HashSet com as perguntas
        HashSet<Question> qst = new HashSet<Question>();
        for(Post p : postsA){
            if(p instanceof Answer){
                p = dump.getPost(((Answer) p).getQuestion());
            }
            qst.add((Question) p);
        }

        //Verifica se a pergunta está no HashSet
        Iterator<Post> iter = postsB.iterator();
        for(int i = 0; iter.hasNext() && i < N;){ 
            Post p = iter.next();
            if(p instanceof Answer){
                p = dump.getPost(((Answer) p).getQuestion());
            }
            if(qst.contains(p)){
                lst.add(p.getId());
                i++;
            }
        }

        return lst;
    }

    /**
     * Query 10
     * Recebe id pergunta, devolve id da resposta com melhor (Scr * 0.65) + (Rep * 0.25) + (Cmt * 0.1) onde:
     *  Scr -> Score (upvotes - downvotes)
     *  Rep -> Reputation do autor
     *  Cmt -> Numero de comentarios
     * Se a pergunta nao for encontrada devolve -1, se nao tiver respostas devolve 0.
     *
     * @param  id   id da pergunta
     *
     * @return      id da resposta, 0 ou -1
     */
    public long betterAnswer(long id){

        Post p = dump.getPost(id);
        if (p == null || p instanceof Answer) return -1;

        double scr, rep, cmt, total, best = 0;
        long bestAns = 0;
        User owner;

        for(Long ans : ((Question) p).getAnswers()){

            Answer answer = (Answer) dump.getPost(ans);

            scr = answer.getScore();
            cmt = answer.getCommentCount();
            owner = dump.getUser(answer.getOwnerId());
            if(owner != null) {
                rep = owner.getReputation();
            } else
                rep = 0;

            total = scr * 0.65 + rep * 0.25 + cmt * 0.1;

            if (total > best){
                best = total;
                bestAns = ans;
            }
        }

        return bestAns;
    }


    /**
     * Query 11
     * Recebe intervalo de tempo, devolve lista com ids das N tags mais usadas no intervalo pelos N users com melhor reputacao.
     * A lista esta por ordem decrescente do numero de vezes que a tag foi usada.
     *
     * @param  N        numero de ids na lista e users considerados
     * @param  begin    data inicial do intervalo
     * @param  end      data final do intervalo
     *
     * @return          lista de ids das tags, nao null
     */
    public List<Long> mostUsedBestRep(int N, LocalDate begin, LocalDate end){

        HashSet<Long> topRepUsersId = new HashSet<>();
        for(User u : dump.getTopUsersByReputation(N)){
            topRepUsersId.add(u.getId());
        }

        HashMap<String, Integer> tagCounter = new HashMap<>();
        //Itera por todas as perguntas do intervalo de tempo
        //Se o dono da pergunta estiver no top as tags sao adicionadas ao tagCounter
        for(Question q : dump.getQuestionsByDate(begin, end)){
            if(topRepUsersId.contains(q.getOwnerId())){
                for(String tag : q.getTags()){
                    tagCounter.compute(tag, (k, v) -> (v == null) ? 0 : v + 1);
                }
            }
        }

        //Ordena as tags pelo numero de vezes que as tags foram utilizadas
        List<Map.Entry<String, Integer>> tagOrder = new ArrayList<>(tagCounter.entrySet());
        tagOrder.sort((a, b) -> - a.getValue().compareTo(b.getValue()));


        List<Long> lst = new ArrayList<>();

        Iterator<Map.Entry<String, Integer>> iter = tagOrder.iterator();
         for(int i = 0; iter.hasNext() && i < N; i++){
             String tagName = iter.next().getKey();
             lst.add(dump.getTagId(tagName));   //converte para id
        }

        return lst;
    }

    public void clear(){}
}