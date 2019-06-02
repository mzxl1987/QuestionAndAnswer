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
 * A thread-safe, mutable sequence of characters.                   //第一句注释就说明SB是线程安全的
 * A string buffer is like a {@link String}, but can be modified. At any
 * point in time it contains some particular sequence of characters, but
 * the length and content of the sequence can be changed through certain
 * method calls.
 * <p>     //接下来这段说明了所有必要的操作都用 synchronized 关键字做了修饰
 * String buffers are safe for use by multiple threads. The methods
 * are synchronized where necessary so that all the operations on any
 * particular instance behave as if they occur in some serial order
 * that is consistent with the order of the method calls made by each of
 * the individual threads involved.
 * <p>
 * The principal operations on a {@code StringBuffer} are the
 * {@code append} and {@code insert} methods, which are
 * overloaded so as to accept data of any type. Each effectively
 * converts a given datum to a string and then appends or inserts the
 * characters of that string to the string buffer. The
 * {@code append} method always adds these characters at the end
 * of the buffer; the {@code insert} method adds the characters at
 * a specified point.
 * <p>
 * For example, if {@code z} refers to a string buffer object
 * whose current contents are {@code "start"}, then
 * the method call {@code z.append("le")} would cause the string
 * buffer to contain {@code "startle"}, whereas
 * {@code z.insert(4, "le")} would alter the string buffer to
 * contain {@code "starlet"}.
 * <p>
 * In general, if sb refers to an instance of a {@code StringBuffer},
 * then {@code sb.append(x)} has the same effect as
 * {@code sb.insert(sb.length(), x)}.
 * <p>
 * Whenever an operation occurs involving a source sequence (such as
 * appending or inserting from a source sequence), this class synchronizes
 * only on the string buffer performing the operation, not on the source.
 * Note that while {@code StringBuffer} is designed to be safe to use
 * concurrently from multiple threads, if the constructor or the
 * {@code append} or {@code insert} operation is passed a source sequence
 * that is shared across threads, the calling code must ensure
 * that the operation has a consistent and unchanging view of the source
 * sequence for the duration of the operation.
 * This could be satisfied by the caller holding a lock during the
 * operation's call, by using an immutable source sequence, or by not
 * sharing the source sequence across threads.
 * <p>
 * Every string buffer has a capacity. As long as the length of the
 * character sequence contained in the string buffer does not exceed
 * the capacity, it is not necessary to allocate a new internal
 * buffer array. If the internal buffer overflows, it is
 * automatically made larger.
 * <p>
 * Unless otherwise noted, passing a {@code null} argument to a constructor
 * or method in this class will cause a {@link NullPointerException} to be
 * thrown.
 * <p>
 * As of  release JDK 5, this class has been supplemented with an equivalent
 * class designed for use by a single thread, {@link StringBuilder}.  The
 * {@code StringBuilder} class should generally be used in preference to
 * this one, as it supports all of the same operations but it is faster, as
 * it performs no synchronization.
 *
 * @author      Arthur van Hoff
 * @see     java.lang.StringBuilder
 * @see     java.lang.String
 * @since   JDK1.0
 */
 public final class StringBuffer
    extends AbstractStringBuilder
    implements java.io.Serializable, CharSequence
{
......
  /**
   * @throws IndexOutOfBoundsException {@inheritDoc}
   * @since      1.5
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
