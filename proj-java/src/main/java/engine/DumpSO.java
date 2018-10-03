 package engine;

import java.time.LocalDate;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;
import java.util.TreeSet;
import java.util.Iterator;
import java.util.Collections;

import common.Pair;

/**
 * Classe que guarda a informacao recolhida de um dump do stack overflow.
 *
 */
public class DumpSO{

    private Map<Long, Post> postsById;
    private TreeSet<Question> questionsByDate;
    private TreeSet<Answer> answersByDate;

    private Map<Long, User> usersById;
    private List<User> usersByPosts;
    private List<User> usersByReputation;

    private Map<String, Long> mapTags;

    /**
     *
     * Contrutor de DumpSO default
     *
     */
    public DumpSO(){
        this.postsById = new HashMap<>();
        this.questionsByDate = new TreeSet<>();
        this.answersByDate = new TreeSet<>();

        this.usersById = new HashMap<>();
        this.usersByPosts = new ArrayList<>();

        this.mapTags = new HashMap<>();
    }

    /**
     *
     * Getter que devolve o apontador para os Posts por Id.
     *
     * @return      Posts por Id.
     *
     */
    public Map<Long, Post> getPostsById(){
        return this.postsById;
    }

    /**
     *
     * Recebe id, devolve User correspondente.
     *
     * @param  id   Id do user
     * 
     * @return      User, null se nao existir
     *
     */
    public User getUser(long id){
    	User user = usersById.get(id);
    	if (user == null) return null;
        return user.clone();
    }
    
    /**
     *
     * Recebe id, devolve Post correspondente.
     *
     * @param  id   Id do post
     * 
     * @return      Post, null se nao existir
     *
     */
    public Post getPost(long id){
        Post p = postsById.get(id);
        if (p == null) return null;
        
        return p.clone();
    }
    
    
    /**
     *
     * Recebe N, devolve lista com N users com mais posts, ordenada pelo numero de posts.
     *
     * @param  N    Numero de users na lista
     * 
     * @return      Lista de users, nao null
     *
     */
    public List<User> getTopUsersByPosts(int N){
        
        List<User> lst = new ArrayList<>();
        
        Iterator<User> iter = usersByPosts.iterator();
        for(int i = 0; iter.hasNext() && i < N; i++){
            lst.add(iter.next().clone());
        }
        
        return lst;
    }


    /**
     *
     * Recebe N, devolve lista com N users com mais reputation, ordenada pelo numero de posts.
     *
     * @param  N    Numero de users na lista
     * 
     * @return      Lista de users, nao null
     *
     */
    public List<User> getTopUsersByReputation(int N){
        
        List<User> lst = new ArrayList<>();
        
        Iterator<User> iter = usersByReputation.iterator();
        for(int i = 0; iter.hasNext() && i < N; i++){
            lst.add(iter.next().clone());
        }
        
        return lst;
    }
    

    /**
     *
     * Recebe intervalo de tempo, devolve perguntas criadas dentro de esse intervalo (ordem cronologica inversa).
     *
     * @param  begin    Data inicial do intervalo
     * @param  end      Data final do intervalo
     * 
     * @return          Lista de perguntas, nao null
     *
     */
    public List<Question> getQuestionsByDate(LocalDate begin, LocalDate end){
        
        List<Question> lst = new ArrayList<>();
        
        for(Question q : questionsByDate.subSet(Question.toDate(end), true, Question.fromDate(begin), true)){
            lst.add(q.clone());
        }
        /*
        for(Question q : questionsByDate.subSet(Question.fromDate(begin), true, Question.toDate(end), true)){
            lst.add(q.clone());
        }
        */
        return lst;
    }
    
    /**
     *
     * Recebe intervalo de tempo, devolve respostas criadas dentro de esse intervalo (ordem cronologica inversa).
     *
     * @param  begin    Data inicial do intervalo
     * @param  end      Data final do intervalo
     * 
     * @return          Lista de respostas, nao null
     *
     */
    public List<Answer> getAnswersByDate(LocalDate begin, LocalDate end){
        
        List<Answer> lst = new ArrayList<>();
        
        for(Answer q : answersByDate.subSet(Answer.toDate(end), true, Answer.fromDate(begin), true)){
            lst.add(q.clone());
        }
        
        return lst;
    }
    
    /**
     *
     * Metodo que devolve uma lista com N perguntas cujo titulo contem a palavra recebi.
     * 
     *
     * @param  N        Numero de perguntas na lista
     * @param  word     Palavra
     * 
     * @return          Lista de perguntas, nao null
     *
     */
    public List<Question> getQuestionsWithWord(int N, String word){
        
        List<Question> lst = new ArrayList<>();
        
        Iterator<Question> iter = questionsByDate.iterator();
        for(int i = 0; iter.hasNext() && i < N;){ 
            Question qst = iter.next();
            if(qst.getTitle().contains(word)){
                lst.add(qst.clone());
                i++;
            }
        }
               
        return lst;
    }
    
    /**
     *
     * Recebe lista de ids, devolve lista de posts correspondente.
     *
     * @param  ids  Lista de ids dos posts
     * 
     * @return      Lista de posts
     *
     */
    public List<Post> getPostsByDate(List<Long> ids){

    	List<Post> lst = new ArrayList<Post>();
        for(Long id : ids){
            lst.add(postsById.get(id));
        }
        
        Collections.sort(lst);

        return lst;
    }

    /**
     *
     * Metodo que recebe uma tag e retorna o id correspondente.
     * 
     * @param  tag  Tag
     * 
     * @return      Id da tag, 0 se nao existir
     *
     */
    public long getTagId(String tag){
    	Long id = mapTags.get(tag);
    	if(id == null) return 0;
        return id;
    }

    /**
     * Recebe id e user correspondente e adiciona ao dump.
     * 
     * @param  id  		Id do user
     * @param  user  	User a ser adicionado
     */
    public void addUser(long id, User user){
        this.usersById.put(id,user);
    }

    /**
     * Guarda todos os users do dump ordenados por numero de posts.
     */
    public void addUsersByPosts(){
        List<User> users = new ArrayList<User>(this.usersById.values());
        users.sort((a,b)->(Integer.compare(b.getPosts().size(), a.getPosts().size())));
        this.usersByPosts = users;
    }

    /**
     * Guarda todos os users do dump ordenados por reputaçao.
     */
    public void addUsersByReputation(){
        List<User> users = new ArrayList<User>(this.usersById.values());
        users.sort((a,b)->(Integer.compare(b.getReputation(), a.getReputation())));
        this.usersByReputation = users;
    }

    /**
     * Recebe id e post correspondente e adiciona ao dump.
     * 
     * @param  id  		Id do post
     * @param  post  	Post a ser adicionado
     *
     */
    public void addPost(long id, Post p){
        this.postsById.put(id, p);
    }

    /**
     *
     * Metodo que adiciona uma resposta ao conjunto de respotas de uma pergunta.
     *
     * @param  a		Answer a ser adionada
     *
     * @return			True se foi adicionada ou false caso contrario
     *
     */
    public boolean addAnsToQST(Answer a){
        if(this.postsById.containsKey(a.getQuestion())){
            Question q = (Question) this.postsById.get(a.getQuestion());
            q.addAnswer(a.getId());
            return true;
        }
        else{
            return false;
        }
    }

    /**
     *
     * Metodo que adiciona o id de um post ao conjunto de posts de um User.
     *
     * @param  id		Id do post a ser adicionado
     * @param  owner	Id do dono (User) do post
     *
     */
    public void addPostToUser(long id, long owner){
        if(this.usersById.containsKey(owner)){
            this.usersById.get(owner).addPost(id);
        }
    }

    /**
     *
     * Metodo que adiciona uma pergunta ao conjunto de perguntas ordenadas por data.
     *
     * @param  q		Perguntas a ser adicionada
     *
     */
    public void addQstByDate(Question q){
        this.questionsByDate.add(q);
    }

    /**
     *
     * Metodo que adiciona uma resposta ao conjunto de respostas ordenadas por data.
     *
     * @param  q		Respostas a ser adicionada
     *
     */
    public void addAnsByDate(Answer a){
        this.answersByDate.add(a);
    }

    /**
     * Recebe id e tag correspondente e adiciona ao dump.
     * 
     * @param  id  		Id da tag
     * @param  tag  	Tag a ser adicionada
     *
     */
    public void addTag(long id, String tag){
        this.mapTags.put(tag, id);
    }
}
