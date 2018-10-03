package engine; 

import java.util.List;
import java.util.ArrayList;

/**
 * Classe que guarda a informacao de um user.
 */
public class User{
    private long id;
    private int reputation;
    private String displayName;
    private String aboutMe;
    private List<Long> posts;

    /**
     *
     * Construtor de User Default
     *
     */
    public User(){
    	this.id = 0;
    	this.reputation = 0;
    	this.displayName = "n/a";
    	this.aboutMe = "n/a";
    	this.posts = new ArrayList<>();
    }

    /**
     *
     * Construtor parametrizado de User
     *
     * @param  id            Id do User
     * @param  reputation    Reputacao do User
     * @param  displayName   Nome do User
     * @param  aboutMe       Descricao do User
     *
     */
    public User(long id, int reputation, String displayName, String aboutMe){
    	this.id = id;
    	this.reputation = reputation;
    	this.displayName = displayName;
    	this.aboutMe = aboutMe;
    	this.posts = new ArrayList<>();
    }

    /**
     * Construtor de User cópia
     *
     * @param  otherUser     User a ser copiado
     */
    public User(User otherUser){
        setId(otherUser.getId());
        setReputation(otherUser.getReputation());
        setDisplayName(otherUser.getDisplayName());
        setAboutMe(otherUser.getAboutMe());
        setPosts(otherUser.getPosts());
    }

    /**
     * Construtor de User que inicializa só com uma lista de posts
     *
     * @param  posts       Lista com os posts que o utilizador pertence
     */
    public User(List<Long> posts){
        this.setPosts(posts);
    }

    /**
     * Obter o id do utilizador
     *
     * @return  Id do User
     */
    public long getId(){
        return this.id;
    }

    /**
     * Definir o Id do utilizador
     *
     * @param  id      Novo id
     */
    public void setId(long id){
        this.id = id;
    }

    /**
     * Obter a reputacao do utilizador
     *
     * @return  Reputacao do utilizador
     */
    public int getReputation(){
        return this.reputation;
    }

    /**
     * Definir a reputacao do utilizador
     *
     * @param rep  Nova reputacao
     */
    public void setReputation(int rep){
        this.reputation = rep;
    }

    /**
     * Obter o nome do utilizador
     *
     * @return  Nome do utilizador
     */
    public String getDisplayName(){
        return this.displayName;
    }

    /**
     * Definir o nome do utilizador
     *
     * @param  nome        Novo nome do user
     */
    public void setDisplayName(String name){
        this.displayName = name;
    }

    /**
     * Obter a descrico do utilizador
     *
     * @return Descricao do utilizador
     */
    public String getAboutMe(){
        return this.aboutMe;
    }

    /**
     * Definir a descricao do utilizador
     * @param  bio     Nova descricao
    */
    public void setAboutMe(String bio){
        this.aboutMe = bio;
    }

    /**
     * Obter a lista dos posts a que o utilizador fez
     * @return  Lista dos posts
     */
    public List<Long> getPosts(){
        return new ArrayList<Long>(posts);
    }

    /**
     * Definir a lista dos posts do utilizador
     *
     * @param  posts       Lista dos posts
     */
    public void setPosts(List<Long> posts){
    	this.posts = new ArrayList<>(posts);
    }

    /**
     * Adiciona um post ao final da lista dos postos que o utilizador fez
     */
    public void addPost(long post){
        this.posts.add(post);
    }


    /**
     *
     * Metodo que junta a informacao de um user numa String e retorna-a
     *
     * @return String com informacao
     */
    public String toString(){
        StringBuilder sb = new StringBuilder("");
        sb.append(this.id).append("\n");
        sb.append(this.reputation).append("\n");
        sb.append(this.displayName).append("\n");
        sb.append("\n");
        return sb.toString();
    }

    /**
     * Duplica o utilizador
     */
    public User clone(){
        return (new User(this));
    }
}
