  # 简述String,StringBuffer,StringBuilder的区别
  * ### String是字符串常量,被final修饰,不可以被继承,创建之后不可以更改,String buffers支持可变string.
  详见 String.class
  ```
  /**
 * ......
 * Strings are constant; their values cannot be changed after they
 * are created. String buffers support mutable strings.
 * ......
 */

  public final class String
      implements java.io.Serializable, Comparable<String>, CharSequence {
      ......
  }
  ```
  * ### StringBuffer是线程安全的,final class,不可以被继承,大多数的方法都使用了synchronized做了修饰.
  ###### StringBuffer.class
  ```
  /**  
 * 第一句注释就说明SB是线程安全的             
 * A thread-safe, mutable sequence of characters.      
 * A string buffer is like a {@link String}, but can be modified. At any
 * point in time it contains some particular sequence of characters, but
 * the length and content of the sequence can be changed through certain
 * method calls.
 * 接下来这段说明了所有必要的操作都用 synchronized 关键字做了修饰
 * <p>     
 * String buffers are safe for use by multiple threads. The methods
 * are synchronized where necessary so that all the operations on any
 * particular instance behave as if they occur in some serial order
 * that is consistent with the order of the method calls made by each of
 * the individual threads involved.
 * ......
 */
 public final class StringBuffer
    extends AbstractStringBuilder
    implements java.io.Serializable, CharSequence
{
......
  /**
   * @throws IndexOutOfBoundsException {@inheritDoc}
   * @since      1.5
   * 该方法会在super.insert()中被回调
   */
  @Override
  public synchronized StringBuffer insert(int dstOffset, CharSequence s,
          int start, int end)
  {
      toStringCache = null;
      super.insert(dstOffset, s, start, end);
      return this;
  }
  @Override
  public synchronized StringBuffer insert(int dstOffset, CharSequence s,
          int start, int end)
  {
      toStringCache = null;
      super.insert(dstOffset, s, start, end);
      return this;
  }

  /**
   * @throws StringIndexOutOfBoundsException {@inheritDoc}
   */
   //当前insert方法中并没有synchronized关键字,他是如何同步的?
  @Override
  public  StringBuffer insert(int offset, boolean b) {
      //请注意这段注释,大体的意思是说,在super class method中将 b 转换成String之后
      //才拿到insert的synchronization权限
      // Note, synchronization achieved via invocation of StringBuffer insert(int, String)
      // after conversion of b to String by super class method
      // Ditto for toStringCache clearing
      super.insert(offset, b);    //请查看super.insert方法
      return this;
  }
  ......
  //StringBuffer将数据放在缓存中
  @Override
  public synchronized String toString() {
      if (toStringCache == null) {
          toStringCache = Arrays.copyOfRange(value, 0, count);
      }
      return new String(toStringCache, true);
  }
  
......
}
  ```
  ###### AbstractStringBuilder.class
  ```
  /**
     * Inserts the specified {@code CharSequence} into this sequence.
     * <p>
     * The characters of the {@code CharSequence} argument are inserted,
     * in order, into this sequence at the indicated offset, moving up
     * any characters originally above that position and increasing the length
     * of this sequence by the length of the argument s.
     * <p>
     * The result of this method is exactly the same as if it were an
     * invocation of this object's
     * {@link #insert(int,CharSequence,int,int) insert}(dstOffset, s, 0, s.length())
     * method.
     *
     * <p>If {@code s} is {@code null}, then the four characters
     * {@code "null"} are inserted into this sequence.
     *
     * @param      dstOffset   the offset.
     * @param      s the sequence to be inserted
     * @return     a reference to this object.
     * @throws     IndexOutOfBoundsException  if the offset is invalid.
     */
    public AbstractStringBuilder insert(int dstOffset, CharSequence s) {
        if (s == null)
            s = "null";
        if (s instanceof String)
            return this.insert(dstOffset, (String)s);        // 该方法在StringBuffer已经重写,并加了synchronized
        return this.insert(dstOffset, s, 0, s.length());     // 该方法在StringBuffer已经重写,并加了synchronized
    }
  ```
  * ### StringBuilder类的实现接口与StringBuffer相同,但是不是线程安全的.
  ###### StringBuilder.class
  ```
  public final class StringBuilder
    extends AbstractStringBuilder
    implements java.io.Serializable, CharSequence
  {
  ......
  }
  ```
  * ### StringBuilder.toString() 与 StringBuffer.toString()方法实现上的区别
  StringBuffer使用了,cache缓存,共享; StringBuilder没有,只是重新new String()
  ###### StringBuffer.class
  ```
  @Override
  public synchronized String toString() {
      if (toStringCache == null) {
          toStringCache = Arrays.copyOfRange(value, 0, count);
      }
      return new String(toStringCache, true);
  }
  ```
  ###### StringBuilder.class
  ```
  @Override
  public String toString() {
      // Create a copy, don't share the array
      return new String(value, 0, count);
  }
  ```
  ###### String.class
  ```
  /*
    * 注释中给了相应的解释,该构造函数使用shares value array 是为了速度.
    * Package private constructor which shares value array for speed.
    * this constructor is always expected to be called with share==true.
    * a separate constructor is needed because we already have a public
    * String(char[]) constructor that makes a copy of the given char[].
    */
    String(char[] value, boolean share) {
        // assert share : "unshared not supported";
        this.value = value;
    }
  ```
