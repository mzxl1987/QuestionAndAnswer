# 简述ArrayList & LinkedList 异同?
# 相同:都实现了List<E>接口
# 差异:
  * ### 1.实现方式的不同
  ###### ArrayList.class
  arraylist的数据都存放在 elementData 的 Object[] 数组中
  ```
  public class ArrayList<E> extends AbstractList<E>
        implements List<E>, RandomAccess, Cloneable, java.io.Serializable
{
    ......
    /**
     * 
     * The array buffer into which the elements of the ArrayList are stored.
     * The capacity of the ArrayList is the length of this array buffer. Any
     * empty ArrayList with elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA
     * will be expanded to DEFAULT_CAPACITY when the first element is added.
     */
    transient Object[] elementData; // non-private to simplify nested class access

    /**
     * The size of the ArrayList (the number of elements it contains).
     *
     * @serial
     */
    private int size;
}
  ```
  ###### LinkedList.class
  只存放Node<E> 类型的 first, last指针,具体数据则存放在了Node<E>节点中，Node<E>是LinkedList的静态内部类.
  Node<E>对象中除了存放element之外还存放了prev,next 2个Node<E>类型的引用
  ```
  public class LinkedList<E>
    extends AbstractSequentialList<E>
    implements List<E>, Deque<E>, Cloneable, java.io.Serializable
{
    transient int size = 0;

    /**
     * Pointer to first node.
     * Invariant: (first == null && last == null) ||
     *            (first.prev == null && first.item != null)
     */
    transient Node<E> first;

    /**
     * Pointer to last node.
     * Invariant: (first == null && last == null) ||
     *            (last.next == null && last.item != null)
     */
    transient Node<E> last;

    ......
    private static class Node<E> {
        E item;
        Node<E> next;
        Node<E> prev;

        Node(Node<E> prev, E element, Node<E> next) {
            this.item = element;
            this.next = next;
            this.prev = prev;
        }
    }

  ```
  * ### ArrayList & LinkedList 在Query,Add,Remove操作上的快慢比较
  * #### ArrayList 时间复杂度O(1), LinkedList 时间复杂度O(N).
  ###### ArrayList.class
  ```
  /**
     * Returns the element at the specified position in this list.
     *
     * @param  index index of the element to return
     * @return the element at the specified position in this list
     * @throws IndexOutOfBoundsException {@inheritDoc}
     */
    public E get(int index) {
        rangeCheck(index);

        return elementData(index);
    }
    
    @SuppressWarnings("unchecked")
    E elementData(int index) {
        return (E) elementData[index];
    }
  ```
  ###### LinkedList.class
  ```
  public E get(int index) {
        checkElementIndex(index);
        return node(index).item;
    }
    
    /**
     * Returns the (non-null) Node at the specified element index.
     */
    Node<E> node(int index) {
        // assert isElementIndex(index);

        if (index < (size >> 1)) {
            Node<E> x = first;
            for (int i = 0; i < index; i++)
                x = x.next;
            return x;
        } else {
            Node<E> x = last;
            for (int i = size - 1; i > index; i--)
                x = x.prev;
            return x;
        }
    }
    
  ```
  * #### 在添加删除操作上,LinkedList 比 ArrayList 块
  LinkedList在修改的操作上只需要修改Node<E>中的first,last引用即可,不需要做额外的操作, 
  而ArrayList在删除数据之后, 需要将数据移动;ArrayList添加数据的时候需要判断数组的长度, 如果长度不够的时候需要做数组的copy
  ###### ArrayList.class
  ```
  /**
     * Appends the specified element to the end of this list.
     *
     * @param e element to be appended to this list
     * @return <tt>true</tt> (as specified by {@link Collection#add})
     */
    public boolean add(E e) {
        ensureCapacityInternal(size + 1);  // Increments modCount!!  增加数组的数量
        elementData[size++] = e;
        return true;
    }
    
    private void ensureCapacityInternal(int minCapacity) {
        if (elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA) {
            minCapacity = Math.max(DEFAULT_CAPACITY, minCapacity);
        }

        ensureExplicitCapacity(minCapacity);
    }
    
    private void ensureExplicitCapacity(int minCapacity) {
        modCount++;

        // overflow-conscious code
        if (minCapacity - elementData.length > 0)
            grow(minCapacity);
    }
    
    /**
     * Increases the capacity to ensure that it can hold at least the
     * number of elements specified by the minimum capacity argument.
     *
     * @param minCapacity the desired minimum capacity
     */
    private void grow(int minCapacity) {
        // overflow-conscious code
        int oldCapacity = elementData.length;
        int newCapacity = oldCapacity + (oldCapacity >> 1);
        if (newCapacity - minCapacity < 0)
            newCapacity = minCapacity;
        if (newCapacity - MAX_ARRAY_SIZE > 0)
            newCapacity = hugeCapacity(minCapacity);
        // minCapacity is usually close to size, so this is a win:
        elementData = Arrays.copyOf(elementData, newCapacity);     //数组的copy
    }
    
    /**
     * 删除相应位置的元素,从指定地方的元素需要向左移动
     * Removes the element at the specified position in this list.
     * Shifts any subsequent elements to the left (subtracts one from their
     * indices).
     *
     * @param index the index of the element to be removed
     * @return the element that was removed from the list
     * @throws IndexOutOfBoundsException {@inheritDoc}
     */
    public E remove(int index) {
        rangeCheck(index);

        modCount++;
        E oldValue = elementData(index);

        int numMoved = size - index - 1;
        if (numMoved > 0)
            System.arraycopy(elementData, index+1, elementData, index,
                             numMoved);           //将右侧的数据向左移动
        elementData[--size] = null; // clear to let GC do its work

        return oldValue;
    }
    
    
  ```
  ###### LinkedList.class
  ```
  
  public boolean add(E e) {
        linkLast(e);    // 只是将e添加到link结尾,不做其他任何操作
        return true;
    }
    
    /**
     * Links e as last element.
     */
    void linkLast(E e) {
        final Node<E> l = last;
        final Node<E> newNode = new Node<>(l, e, null);
        last = newNode;
        if (l == null)
            first = newNode;
        else
            l.next = newNode;
        size++;
        modCount++;
    }
  
  public E remove(int index) {
        checkElementIndex(index);
        return unlink(node(index));
    }
    /**
     * 将节点从链表上取下
     * Unlinks non-null node x.
     */
    E unlink(Node<E> x) {
        // assert x != null;
        final E element = x.item;
        final Node<E> next = x.next;
        final Node<E> prev = x.prev;

        if (prev == null) {
            first = next;
        } else {
            prev.next = next;
            x.prev = null;
        }

        if (next == null) {
            last = prev;
        } else {
            next.prev = prev;
            x.next = null;
        }

        x.item = null;
        size--;
        modCount++;
        return element;
    }
  ```
  * ### LinkedList 比 ArrayList 更占内存
  ArrayList 存储对象本身, LinkedList 存储的是Node<E>, Node<E>中除了对象本身之外,还有 prev,next 2个Node<E>类型的引用
  
