template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie
{
    std::shared_ptr<const TrieNode> grandparent = nullptr;
    std::shared_ptr<const TrieNode> root_node = root_ ? root_->Clone() : nullptr;
    std::shared_ptr<const TrieNode> parent = nullptr;
    char ch = 0;
    auto node = root_node;
    // key=null
    if (key.empty())
    {
        // 处理空字符串键
        auto mutableNode = std::const_pointer_cast<TrieNode>(root_node);
        auto valueNode = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(mutableNode);
        if (valueNode)
        {
            // 如果根节点已经是一个值节点，更新其值
            valueNode->value_ = std::make_shared<T>(std::move(value));
        }
        else
        {
            // 否则，创建一个新的值节点
            auto newvalue = std::make_shared<TrieNodeWithValue<T>>(mutableNode->children_, std::make_shared<T>(std::move(value)));
            root_node = newvalue;
        }
        return Trie(root_node);
    }
    // 遍历键中的每个字符
    for (size_t i = 0; i < key.size(); ++i)
    {

        ch = key[i];

        if (node && node->children_.find(ch) != node->children_.end())
        {
            // 移动到对应的子节点
            parent = node;
            grandparent = node;
            std::unique_ptr<TrieNode> cloned_node = node->children_.at(ch)->Clone();
            node = std::shared_ptr<TrieNode>(std::move(cloned_node));
            auto par = std::const_pointer_cast<TrieNode>(parent);
            auto grand_par = std::const_pointer_cast<TrieNode>(grandparent);
            par->children_[ch] = node;
            grand_par->children_[ch] = node;
        }
        else
        {
            // 创建新节点
            auto newNode = std::make_shared<const TrieNode>(std::map<char, std::shared_ptr<const TrieNode>>());
            grandparent = node;
            node = newNode;
            auto mutableParent = std::const_pointer_cast<TrieNode>(parent);       // 非值节点
            auto mutable_Parent = std::const_pointer_cast<TrieNode>(grandparent); // 值节点
            if (parent)
            {
                // 更新父节点的子节点映射
                if (mutableParent->is_value_node_ && mutable_Parent->is_value_node_)
                {
                    mutable_Parent->children_[ch] = newNode;
                    auto valueNode = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(mutable_Parent);
                    std::make_shared<const TrieNodeWithValue<T>>(mutable_Parent->children_, valueNode->value_);
                }
                else if (mutableParent->is_value_node_)
                {
                    mutableParent->children_[ch] = newNode;
                    auto valueNode = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(mutableParent);
                    std::make_shared<const TrieNodeWithValue<T>>(mutableParent->children_, valueNode->value_);
                }
                else
                {
                    mutable_Parent->children_[ch] = newNode;
                }
            }
            else
            {
                // 更新根节点
                if (root_ == nullptr)
                {
                    // 根节点为空，创建新的根节点
                    auto new_Node = std::make_shared<const TrieNode>(std::map<char, std::shared_ptr<const TrieNode>>());
                    auto root_child = std::const_pointer_cast<TrieNode>(new_Node);
                    root_child->children_[ch] = newNode;
                    root_node = new_Node;
                    node = newNode;
                    parent = new_Node;
                    grandparent = new_Node;
                }
                else
                {
                    // 根节点不为空，复制根节点以确保不影响现有节点
                    std::unique_ptr<TrieNode> cloned_node = root_->Clone();
                    auto newRoot = std::shared_ptr<TrieNode>(std::move(cloned_node));
                    newRoot->children_[ch] = newNode; // 添加新的映射
                    root_node = newRoot;              // 设置新的根节点
                }
            }
            parent = node;
        }
    }
    // 在最终的节点处设置值

    if (node)
    {
        auto mutableNode = std::const_pointer_cast<TrieNode>(node);
        if (mutableNode->is_value_node_)
        {
            // 如果最后一个节点是值节点，更新其值
            auto valueNode = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(mutableNode);
            if (valueNode)
            {
                valueNode->value_ = std::make_shared<T>(std::move(value));
            }
            else
            {
                auto newvalue = std::make_shared<TrieNodeWithValue<T>>(mutableNode->children_, std::make_shared<T>(std::move(value)));
                std::const_pointer_cast<TrieNode>(grandparent)->children_[ch] = newvalue;
            }
        }
        else
        {
            // 如果最后一个节点是非值节点，转换为值节点并赋值
            auto newNodeWithValue = std::make_shared<TrieNodeWithValue<T>>(mutableNode->children_, std::make_shared<T>(std::move(value)));
            if (grandparent)
            {
                std::const_pointer_cast<TrieNode>(grandparent)->children_[ch] = newNodeWithValue;
            }
            else
            {
                root_node = newNodeWithValue;
            }
        }
    }
    return Trie(root_node);
}
