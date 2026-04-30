from app.repositories import task_repo

def get_leaf_tasks(tasks):
    # (title, parent_id, datetime.now(), deadline))
    task_ids = {t[0] for t in tasks}
    parent_ids = {t[2] for t in tasks if t[2] is not None}

    leaf_ids = task_ids - parent_ids
    return [t for t in tasks if t[0] in leaf_ids]

def get_progress(tasks=None):
    if tasks is None:
        tasks = task_repo.get_all_tasks()
    #lấy các leaf task
    leaf_tasks = get_leaf_tasks(tasks)

    if not leaf_tasks:
        return 0

    done = sum(1 for t in leaf_tasks if t[5] == 1)
    total = len(leaf_tasks)

    return round(done / total * 100, 2)

def get_task_progress(task_id, tasks=None):
    if tasks is None:
        tasks = task_repo.get_all_tasks()

    by_parent = {}
    by_id = {}
    for task in tasks:
        by_id[task[0]] = task
        by_parent.setdefault(task[2], []).append(task)

    root = by_id.get(task_id)
    if root is None:
        return 0

    subtree = []
    stack = [root]
    while stack:
        node = stack.pop()
        subtree.append(node)
        stack.extend(by_parent.get(node[0], []))

    return get_progress(subtree)

def set_done(task_id, is_done):
    task_repo.update_done(task_id, 1 if is_done else 0)
