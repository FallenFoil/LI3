 package engine;

import java.time.LocalDate;

/**
 * Classe abstrata que guarda a informacao de um post.
 */
public abstract class Post implements Comparable<Post>{

    private long id;
    private LocalDate creationDate;
    private long ownerId;

    

    /**
     * Construtor de Post Default.
     */
    public Post(){
      this.id = 0;
      this.creationDate = null;
      this.ownerId = 0;
    }

    /**
     * Construtor de Post cópia.
     *
     * @param  Post
     */
    public Post(Post p){
      this.id = p.getId();
      this.creationDate = p.getCreationDate();
      this.ownerId = p.getOwnerId();
    }

    /**
     * Construtor parametrizado de Post.
     *
     * @param  id        Id do Post
     * @param  date      Data do Post
     * @param  ownerId   Id do autor
     */
    public Post(long id, LocalDate date, long ownerId){
      this.id = id;
      this.creationDate = date;
      this.ownerId = ownerId;
    }

    /**
     * Obter o id do Post.
     *
     * @return Id do Post
     */
    public long getId(){
      return this.id;
    }

    /**
     * Obter a data de criação do Post.
     *
     * @return Data de criacao do post
     */
    public LocalDate getCreationDate(){
      return this.creationDate;
    }

    /**
     * Obter o Id do criador do Post.
     *
     * @return  Id do autor
     */
    public long getOwnerId(){
      return this.ownerId;
    }

    /**
     * Definir o Id do Post.
     *
     * @param  id    Novo id do Post
     */
    public void setId(long id){
      this.id = id;
    }

    /**
     * Definir a data de criação do Post.
     *
     * @param  date  Nova data de criacao
     */
    public void setCreationDate(LocalDate date){
      this.creationDate = date;
    }

    /**
     * Definir o utilizador que criou o Post.
     *
     * @param ownerId   Novo id do autor
     */
    public void setOwnerId(long ownerId){
      this.ownerId = ownerId;
    }

    /**
     * Devolve o valor hashcode do User, que é o id.
     *
     * @return   valor hashcode
     */
    public int hashCode(){
        return Long.hashCode(this.id);
    }

    /**
     * Indica se algum outro objeto é igual a este
     *
     * @param  o    Objeto com que compara
     *
     * @return      True se forem iguais, false no case contrario
     */
    public boolean equals(Object o){
        if(this == o) return true;

        if(o == null || this.getClass() != o.getClass())
            return false;

        Post that = (Post) o;
        return this.id == that.id;
    }

    /**
     *
     * Cria e junta a informacao de um post numa String
     *
     * @return  String com a informacao
     */
    public String toString(){
        StringBuilder sb = new StringBuilder("");
        sb.append(id).append("\n");
        sb.append(ownerId).append("\n");
        return sb.toString();
    }

    /**
     * Compara com outro post baseando-se na data.
     * Se a data for igual compara os ids.
     *
     * @param o  objeto com que compara
     *
     * @return   0 se iguais, -1 se e antes e 1 se e depois
     */
    public int compareTo(Post that){
        final int BEFORE = -1;
        final int EQUAL = 0;
        final int AFTER = 1;

        if (this.id == that.id) return EQUAL; //consistente com equals

        if (this.creationDate.equals(that.creationDate)) {
            if(this.id > that.id) return BEFORE;
            return AFTER;
        }

        if (this.creationDate.isAfter(that.creationDate)) return BEFORE;
        return AFTER;
    }

    public abstract Post clone();
}
