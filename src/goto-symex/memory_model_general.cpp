/*******************************************************************\

Module: Memory models for C11 memory model

(would be later extended to all memory models)

Author: Zhihang Sun, sunzh20@mails.tsinghua.edu.cn

\*******************************************************************/

#include "memory_model_general.h"
#include <util/find_symbols.h>

#include <iostream>

void memory_model_generalt::operator()(symex_target_equationt &equation, message_handlert &message_handler)
{
    messaget log{message_handler};
    log.statistics() << "Adding C11 constraints" << messaget::eom;

    build_event_lists(equation, message_handler);
    build_clock_type();

    build_labels(equation);
    build_lkmm_nesting(equation);
    build_loc(equation);
    build_thd(equation);
    build_id(equation);

    build_po(equation);
    build_co(equation);
    build_rf(equation);

    std::set<std::string> reads;
    build_shared_reads(equation, reads);
    build_data(equation, reads);
    build_addr(equation, reads);
    build_ctrl(equation, reads);

    build_lkmm_crit(equation);

    if(enable_cutting && really_need_cutting())
    {
        build_cutting(equation);
        cat.remove_cut_propagate_edges();
    }

    equation.cat = cat;
    equation.use_cat = true;
    cat.show();

    // std::cout << "memory model is built!\n";
    // equation.output(std::cout);
}

//useless now
exprt memory_model_generalt::before(event_it e1, event_it e2)
{
  return partial_order_concurrencyt::before(
    e1, e2, AX_PROPAGATION);
}

void memory_model_generalt::add_necessary_oc_edge(symex_target_equationt &equation, event_it e1, event_it e2, std::string kind, exprt guard_expr)
{
    if(guard_expr == true_exprt() && strict_guard)
        guard_expr = simplify_expr(and_exprt(e1->guard, e2->guard), ns);

    add_necessary_oc_edge(equation, get_name(e1), get_name(e2), kind, guard_expr);
}

void memory_model_generalt::add_necessary_oc_edge(symex_target_equationt &equation, std::string e1_str, std::string e2_str, std::string kind, exprt guard_expr)
{
    if(cat.binary_relations.find(kind) == cat.binary_relations.end())
        return;

    if(guard_expr == true_exprt() && strict_guard)
    {
        exprt guard1 = true_exprt();
        if(id_to_guard.find(e1_str) != id_to_guard.end())
            guard1 = id_to_guard[e1_str];
        exprt guard2 = true_exprt();
        if(id_to_guard.find(e2_str) != id_to_guard.end())
            guard2 = id_to_guard[e2_str];
        guard_expr = simplify_expr(and_exprt(guard1, guard2), ns);
    }
    add_oc_edge(equation, e1_str, e2_str, kind, guard_expr);
}

void memory_model_generalt::add_necessary_oc_label(symex_target_equationt &equation, event_it e, std::string label, exprt guard_expr)
{
    if(guard_expr == true_exprt() && strict_guard)
        guard_expr = simplify_expr(e->guard, ns);

    add_necessary_oc_label(equation, get_name(e), label, guard_expr);
}

void memory_model_generalt::add_necessary_oc_label(symex_target_equationt &equation, std::string e_str, std::string label, exprt guard_expr)
{
    if(cat.unary_relations.find(label) == cat.unary_relations.end())
        return;

    if(guard_expr == true_exprt() && strict_guard)
    {
        if(id_to_guard.find(e_str) != id_to_guard.end())
            guard_expr = simplify_expr(id_to_guard[e_str], ns);
    }

    add_oc_label(equation, e_str, label, guard_expr);
}

void memory_model_generalt::add_apo(symex_target_equationt &equation, event_it e1, event_it e2, std::set<event_it>& added_rmw_labels)
{
    add_necessary_oc_edge(equation, e1, e2, "rmw", true_exprt());
    if(added_rmw_labels.find(e1) == added_rmw_labels.end())
    {
        added_rmw_labels.insert(e1);
        add_necessary_oc_label(equation, e1, "RMW", true_exprt());
        if(e1->is_shared_write())
            add_necessary_oc_label(equation, e1, "STRONG", true_exprt());
    }
    if(added_rmw_labels.find(e2) == added_rmw_labels.end())
    {
        added_rmw_labels.insert(e2);
        add_necessary_oc_label(equation, e2, "RMW", true_exprt());
        if(e2->is_shared_write())
            add_necessary_oc_label(equation, e2, "STRONG", true_exprt());
    }
}

/*typedef enum
{
    memory_order_relaxed = 0,
    memory_order_consume = 1,
    memory_order_acquire = 2,
    memory_order_release = 3,
    memory_order_acq_rel = 4,
    memory_order_seq_cst = 5
} memory_order;*/

std::vector<std::string> c11_mos{"RLX", "CON", "ACQ", "REL", "ACQ_REL", "SC"};

/* typedef enum
{
    memory_order_relaxed,
    memory_order_once,
    memory_order_acquire,
    memory_order_release,
    mb,
    wmb,
    rmb,
    rcu_lock,
    rcu_unlock,
    rcu_sync,
    before_atomic,
    after_atomic,
    after_spinlock,
    barrier,
} memory_order; */

std::vector<std::string> lkmm_mos{"Relaxed", "Once", "Acquire", "Release", "Mb", "Wmb", "Rmb",
    "Rcu-lock", "Rcu-unlock", "Sync-rcu",
    "Before-atomic", "After-atomic", "After-spinlock", "Barrier", "After-unlock-lock"};

void memory_model_generalt::build_labels(symex_target_equationt& equation)
{
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
    {
        if(e_it->is_shared_read())
        {
            add_necessary_oc_label(equation, e_it, "R", true_exprt());
            add_necessary_oc_label(equation, e_it, "M", true_exprt());
            add_necessary_oc_label(equation, e_it, "_", true_exprt());
        }
        else if(e_it->is_shared_write())
        {
            add_necessary_oc_label(equation, e_it, "W", true_exprt());
            add_necessary_oc_label(equation, e_it, "M", true_exprt());
            add_necessary_oc_label(equation, e_it, "_", true_exprt());
            if(e_it->is_init())
                add_necessary_oc_label(equation, e_it, "IW", true_exprt());
        }
        else if(e_it->is_memory_barrier())
        {
            add_necessary_oc_label(equation, e_it, "F", true_exprt());
            add_necessary_oc_label(equation, e_it, "~M", true_exprt());
            add_necessary_oc_label(equation, e_it, "_", true_exprt());

            // Here we model sync/lwsync/isync as fences: Fsync, Flwsync, Fisync.
            // Binary sync relations are modeled as:
            // let sync = [M]; po; [Fsync]; po; [M]
            // let lwsync = ([R]; po; [Flwsync]; po; [M]) | ([M]; po; [Flwsync]; po; [W])
            // let ctrlisync = [R]; ctrl; [Fisync]; po
            // Please manually add those definitions into cat modules.
            std::string function_id = e_it->source.function_id.c_str();
            if(function_id == "fence" || function_id == "sync")
                add_necessary_oc_label(equation, e_it, "Fsync", true_exprt());
            if(function_id == "lwfence" || function_id == "lwsync")
                add_necessary_oc_label(equation, e_it, "Flwsync", true_exprt());
            if(function_id == "isync")
                add_necessary_oc_label(equation, e_it, "Fisync", true_exprt());
        }

        bool is_c11 = false;
        bool is_lkmm = false;
        std::string function_id = id2string(e_it->source.function_id);

        if(e_it->is_shared_write())
        {
            if(function_id.find("__atomic_store") != std::string::npos ||
                function_id.find("__atomic_fetch") != std::string::npos ||
                function_id.find("__atomic_exchange") != std::string::npos ||
                function_id.find("__atomic_compare_exchange") != std::string::npos)
            {
                add_necessary_oc_label(equation, e_it, "A", true_exprt());
                is_c11 = true;
            }
            if(function_id == "__LKMM_STORE")
                is_lkmm = true;
        }

        if(e_it->is_shared_read())
        {
            if(function_id.find("__atomic_load") != std::string::npos ||
                function_id.find("__atomic_fetch") != std::string::npos ||
                function_id.find("__atomic_exchange") != std::string::npos ||
                function_id.find("__atomic_compare_exchange") != std::string::npos)
            {
                add_necessary_oc_label(equation, e_it, "A", true_exprt());
                is_c11 = true;
            }
            if(function_id == "__LKMM_LOAD")
                is_lkmm = true;
        }

        if(e_it->is_memory_barrier())
        {
            if(function_id == "__atomic_thread_fence")
            {
                add_necessary_oc_label(equation, e_it, "A", true_exprt());
                is_c11 = true;
            }
            if(function_id == "__LKMM_FENCE")
                is_lkmm = true;
        }
        
        if(is_c11 || is_lkmm)
        {
            for(auto e_it2 = e_it; e_it2 != equation.SSA_steps.begin(); e_it2--)
            {
                if(!e_it2->is_assignment())
                    continue;
                auto lhs = e_it2->ssa_lhs;
                auto rhs = e_it2->ssa_rhs;
                std::string lhs_id = lhs.get_identifier().c_str();
                auto& mos = is_c11 ? c11_mos : lkmm_mos; // assume not both!

                if(lhs_id.find("::memorder") != std::string::npos && lhs_id.find(function_id) != std::string::npos && rhs.id() == ID_constant)
                {
                    int mo = string2integer(rhs.get_string(ID_value), 16).to_long();
                    if(mo < 0 || mo >= int(mos.size()))
                    {
                        std::cout << "ERROR: Unsupported memorder " << mo << "\n";
                        std::exit(255);
                    }

                    std::string mo_label = mos[mo];
                    if(is_c11)
                    {
                        if(e_it->is_shared_write() && mo_label == "ACQ_REL")
                            mo_label = "REL";
                        if(e_it->is_shared_write() && mo_label == "ACQ")
                            mo_label = "RLX";
                        if(e_it->is_shared_read() && mo_label == "ACQ_REL")
                            mo_label = "ACQ";
                        if(e_it->is_shared_read() && mo_label == "REL")
                            mo_label = "RLX";
                    }
                    add_necessary_oc_label(equation, e_it, mo_label, true_exprt());

                    if(mos[mo] == "Rcu-lock")
                        lkmm_locks.push_back(e_it);
                    if(mos[mo] == "Rcu-unlock")
                        lkmm_unlocks.push_back(e_it);

                    break;
                }
            }
            
        }
    }
}

void memory_model_generalt::build_lkmm_nesting(symex_target_equationt &equation)
{
    bool in_spin_lock = false;
    bool in_spin_unlock = false;
    bool in_atomic_op = false;
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
    {
        if(e_it->is_function_call() && e_it->called_function == "__LKMM_SPIN_LOCK")
        {
            std::cout << "in spin lock\n";
            in_spin_lock = true;
        }
        if(e_it->is_function_return() && e_it->source.function_id == "__LKMM_SPIN_LOCK")
        {
            std::cout << "out of spin lock\n";
            in_spin_lock = false;
        }
        if(e_it->is_function_call() && e_it->called_function == "__LKMM_SPIN_UNLOCK")
        {
            std::cout << "in spin unlock\n";
            in_spin_unlock = true;
        }
        if(e_it->is_function_return() && e_it->source.function_id == "__LKMM_SPIN_UNLOCK")
        {
            std::cout << "out of spin unlock\n";
            in_spin_unlock = false;
        }
        if(e_it->is_function_call() && e_it->called_function == "__LKMM_ATOMIC_OP")
        {
            std::cout << "in atomic op\n";
            in_atomic_op = true;
        }
        if(e_it->is_function_return() && e_it->source.function_id == "__LKMM_ATOMIC_OP")
        {
            std::cout << "out of atomic op\n";
            in_atomic_op = false;
        }

        if(in_spin_lock && e_it->is_shared_read())
            add_necessary_oc_label(equation, e_it, "LKR", true_exprt());
        if(in_spin_lock && e_it->is_shared_write())
            add_necessary_oc_label(equation, e_it, "LKW", true_exprt());
        if(in_spin_unlock && (e_it->is_shared_write() || e_it->is_shared_read()))
            add_necessary_oc_label(equation, e_it, "UL", true_exprt());

        if(in_atomic_op && e_it->is_shared_read())
            add_necessary_oc_label(equation, e_it, "Noreturn", true_exprt());
        else if(e_it->is_shared_read() || e_it->is_shared_write() || e_it->is_memory_barrier())
            add_necessary_oc_label(equation, e_it, "~Noreturn", true_exprt());
    }
}

void memory_model_generalt::build_loc(symex_target_equationt &equation)
{
    per_loc_mapt per_loc_map;
    build_per_loc_map(equation, per_loc_map);

    for(auto& pair: per_loc_map)
    {
        auto& list = pair.second;
        for(auto e_it1 = list.begin(); e_it1 != list.end(); e_it1++)
            for(auto e_it2 = list.begin(); e_it2 != list.end(); e_it2++)
                add_necessary_oc_edge(equation, *e_it1, *e_it2, "loc", true_exprt());
    }
}

void memory_model_generalt::build_thd(symex_target_equationt &equation)
{
    per_thread_mapt per_thread_map;
    build_per_thread_map(equation, per_thread_map);

    // internal
    for(auto& pair: per_thread_map)
    {
        auto& list = pair.second;
        for(auto e_it1 = list.begin(); e_it1 != list.end(); e_it1++)
            for(auto e_it2 = list.begin(); e_it2 != list.end(); e_it2++)
                add_necessary_oc_edge(equation, *e_it1, *e_it2, "int", true_exprt());
    }

    // external
    for(auto& pair1: per_thread_map)
        for(auto& pair2: per_thread_map)
        {
            if(pair1.first == pair2.first)
                continue;
            auto& list1 = pair1.second;
            auto& list2 = pair2.second;
        for(auto e_it1 = list1.begin(); e_it1 != list1.end(); e_it1++)
            for(auto e_it2 = list2.begin(); e_it2 != list2.end(); e_it2++)
                add_necessary_oc_edge(equation, *e_it1, *e_it2, "ext", true_exprt());
        }
}

void memory_model_generalt::build_id(symex_target_equationt &equation)
{
    for(auto e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
    {
        if(e_it->is_shared_write() || e_it->is_shared_read() || e_it->is_memory_barrier() || e_it->is_spawn())
            add_necessary_oc_edge(equation, e_it, e_it, "id", true_exprt());
    }
}

void add_po_closure(std::vector<std::string>& event_names, std::map<std::string, int>& event_name_to_id, 
    std::vector<std::set<int>>& outs, std::vector<std::set<int>>& ins, std::string e1, std::string e2)
{
    int e1_id, e2_id;
    if(event_name_to_id.find(e1) != event_name_to_id.end())
        e1_id = event_name_to_id[e1];
    else
    {
        e1_id = event_names.size();
        event_names.push_back(e1);
        event_name_to_id[e1] = e1_id;
        outs.push_back(std::set<int>());
        ins.push_back(std::set<int>());
    }
    if(event_name_to_id.find(e2) != event_name_to_id.end())
        e2_id = event_name_to_id[e2];
    else
    {
        e2_id = event_names.size();
        event_names.push_back(e2);
        event_name_to_id[e2] = e2_id;
        outs.push_back(std::set<int>());
        ins.push_back(std::set<int>());
    }
    std::set<int> e1_ins = ins[e1_id];
    e1_ins.insert(e1_id);
    std::set<int> e2_outs = outs[e2_id];
    e2_outs.insert(e2_id);
    for(auto e1_in : e1_ins)
        for(auto e2_out : e2_outs)
        {
            outs[e1_in].insert(e2_out);
            ins[e2_out].insert(e1_in);
        }
}

bool memory_model_generalt::is_mm_apo(event_it e1, event_it e2)
{
    if(!e1->is_shared_read() && !e1->is_shared_write())
        return false;
    if(!e2->is_shared_read() && !e2->is_shared_write())
        return false;
    auto e1_original_name = remove_level_2(e1->ssa_lhs).get_identifier();
    auto e2_original_name = remove_level_2(e2->ssa_lhs).get_identifier();
    if(e1_original_name != e2_original_name)
        return false;
    return is_apo(e1, e2);
}

void memory_model_generalt::build_po(symex_target_equationt &equation)
{
    std::vector<std::string> event_names;
    std::map<std::string, int> event_name_to_id;
    std::vector<std::set<int>> outs;
    std::vector<std::set<int>> ins;

    per_thread_mapt per_thread_map;
    build_per_thread_map(equation, per_thread_map, true);

    //thread spawn
    std::vector<event_it> join_events;
    for(auto e_it = equation.SSA_steps.begin(); e_it != equation.SSA_steps.end(); e_it++)
    {
        if(!e_it->is_shared_read())
            continue;
        std::string read_name = id2string(id(e_it));
        if(read_name.find("__CPROVER_threads_exited") == std::string::npos)
            continue;
        
        std::string function_name = e_it->source.function_id.c_str();
        if(function_name.find("pthread_join") == std::string::npos)
            continue;
        
        join_events.push_back(e_it);
    }

    std::set<event_it> added_rmw_labels;

    unsigned next_thread_id=0;
    for(auto e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
    {
        if(e_it->is_spawn())
        {
            auto next_thread = per_thread_map.find(++next_thread_id);
            if(next_thread == per_thread_map.end())
                continue;

            // add a constraint for all events,
            // considering regression/cbmc-concurrency/pthread_create_tso1
            for(auto n_it=next_thread->second.begin(); n_it!=next_thread->second.end(); n_it++)
            {
                if(!(*n_it)->is_memory_barrier())
                {
                    add_po_closure(event_names, event_name_to_id, outs, ins, get_name(e_it), get_name(*n_it));
                    // add_necessary_oc_edge(equation, e_it, *n_it, "po", true_exprt());
                    if(is_mm_apo(e_it, *n_it))
                        add_apo(equation, e_it, *n_it, added_rmw_labels);
                }
            }

            int join_id = int(next_thread_id) - 1;
            if(join_id < int(join_events.size()))
            {
                auto join_event = join_events[join_id];
                for(auto n_it=next_thread->second.begin(); n_it!=next_thread->second.end(); n_it++)
                {
                    if(!(*n_it)->is_memory_barrier())
                    {
                        add_po_closure(event_names, event_name_to_id, outs, ins, get_name(*n_it), get_name(join_event));
                        // add_necessary_oc_edge(equation, *n_it, join_event, "po", true_exprt());
                        if(is_mm_apo(e_it, *n_it))
                            add_apo(equation, *n_it, join_event, added_rmw_labels);
                    }
                }
            }

        }
    }

    build_guard_map_all(equation);

    // iterate over threads
    for(auto t_it=per_thread_map.begin(); t_it!=per_thread_map.end(); t_it++)
    {
        const event_listt &events=t_it->second;

        // this lists all program order

        for(auto e1_it = events.begin(); e1_it != events.end(); e1_it++)
            for(auto e2_it = e1_it; e2_it != events.end(); e2_it++)
            {
                if(e1_it == e2_it)
                    continue;
                add_po_closure(event_names, event_name_to_id, outs, ins, get_name(*e1_it), get_name(*e2_it));
                // add_necessary_oc_edge(equation, *e1_it, *e2_it, "po", true_exprt());
                if(is_mm_apo(*e1_it, *e2_it))
                    add_apo(equation, *e1_it, *e2_it, added_rmw_labels);
            }
        // this ignores those can be derived through transitivity
        // event_it previous=equation.SSA_steps.end();
        // for(auto e_it=events.begin(); e_it!=events.end(); e_it++)
        // {
        //     if((*e_it)->is_memory_barrier())
        //         continue;
        //     if(previous==equation.SSA_steps.end())
        //     {
        //         // first one?
        //         previous=*e_it;
        //         continue;
        //     }
        //     add_necessary_oc_edge(equation, previous, *e_it, "po", true_exprt());
        //     if(is_mm_apo(previous, *e_it))
        //         add_apo(equation, previous, *e_it, added_rmw_labels);
        //     previous=*e_it;
        // }
    }

    for(int e1_id = 0; e1_id < int(event_names.size()); e1_id++)
    {
        for(int e2_id : outs[e1_id])
        {
            std::string e1 = event_names[e1_id];
            std::string e2 = event_names[e2_id];
            add_necessary_oc_edge(equation, e1, e2, "po", true_exprt());
        }
    }

    // for __CPROVER variable accesses, we always consider them as RMW and Atomic
    for(auto t_it=per_thread_map.begin(); t_it!=per_thread_map.end(); t_it++)
    {
        const event_listt &events=t_it->second;
        for(auto e_it = events.begin(); e_it != events.end(); e_it++)
        {
            if(!(*e_it)->is_shared_write() && !(*e_it)->is_shared_read())
                continue;
            auto str = get_name(*e_it);
            if(str.find("__CPROVER") != std::string::npos && added_rmw_labels.find(*e_it) == added_rmw_labels.end())
                add_necessary_oc_label(equation, *e_it, "RMW", true_exprt());
            if(str.find("__CPROVER") != std::string::npos)
                add_necessary_oc_label(equation, *e_it, "A", true_exprt());
        }
    }
}

void memory_model_generalt::build_rf(symex_target_equationt &equation)
{
    memory_model_baset::read_from(equation);
}

void memory_model_generalt::build_co(symex_target_equationt &equation)
{
    for(auto a_it=address_map.begin(); a_it!=address_map.end(); a_it++)
    {
        const a_rect &a_rec=a_it->second;

        // This is quadratic in the number of writes
        // per address. Perhaps some better encoding
        // based on 'places'?
        for(auto w_it1=a_rec.writes.begin(); w_it1!=a_rec.writes.end(); ++w_it1)
        {
            auto next=w_it1;
            ++next;

            for(auto w_it2=next; w_it2!=a_rec.writes.end(); ++w_it2)
            {
                // ws is a total order, no two elements have the same rank
                // s -> w_evt1 before w_evt2; !s -> w_evt2 before w_evt1

                bool can12 = !(assume_local_consistency && po(*w_it2, *w_it1));
                bool can21 = !(assume_local_consistency && po(*w_it1, *w_it2));
                if((*w_it1)->is_init())
                    can21 = false;
                if((*w_it2)->is_init())
                    can12 = false;

                if(can12 && can21)
                {
                    symbol_exprt s12=nondet_bool_symbol("co");
                    symbol_exprt s21=nondet_bool_symbol("co");

                    add_constraint(equation, equal_exprt(or_exprt(s12, s21), and_exprt((*w_it1)->guard, (*w_it2)->guard)), "co-some", (*w_it1)->source);
                    add_constraint(equation, not_exprt(and_exprt(s12, s21)), "co-only-one", (*w_it1)->source);
                    add_necessary_oc_edge(equation, *w_it1, *w_it2, "co", s12);
                    add_necessary_oc_edge(equation, *w_it2, *w_it1, "co", s21);
                }
                else if(can12)
                {
                    symbol_exprt s12=nondet_bool_symbol("co");
                    add_constraint(equation, equal_exprt(s12, and_exprt((*w_it1)->guard, (*w_it2)->guard)), "co-some", (*w_it1)->source);
                    add_necessary_oc_edge(equation, *w_it1, *w_it2, "co", s12);
                }
                else if(can21)
                {
                    symbol_exprt s21=nondet_bool_symbol("co");
                    add_constraint(equation, equal_exprt(s21, and_exprt((*w_it2)->guard, (*w_it1)->guard)), "co-some", (*w_it1)->source);
                    add_necessary_oc_edge(equation, *w_it2, *w_it1, "co", s21);
                }
                else
                    std::cout << "WARNING: either " << id(*w_it1) << " or " << id(*w_it2) << " cannot coherence before each other.\n";
            }
        }
    }
}

void memory_model_generalt::build_shared_reads(symex_target_equationt &equation, std::set<std::string>& reads)
{
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_shared_read())
            reads.insert(id2string(id(e_it)));
}

void memory_model_generalt::build_guard_map(symex_target_equationt& equation, std::map<symbol_exprt, exprt>& guard_map)
{
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_assignment() && e_it->assignment_type == symex_targett::assignment_typet::GUARD)
            guard_map[e_it->ssa_lhs] = e_it->ssa_rhs;
}

void memory_model_generalt::build_data(symex_target_equationt &equation, std::set<std::string>& reads)
{
    std::map<symbol_exprt, std::set<symbol_exprt>> assignment_symbol_map;
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_assignment())
            assignment_symbol_map[to_symbol_expr(e_it->ssa_lhs)] = find_symbols(e_it->ssa_rhs);
    
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_shared_write())
        {
            symbol_exprt lhs = e_it->ssa_lhs;
            if(assignment_symbol_map.find(lhs) == assignment_symbol_map.end())
                continue;

            std::string e_str = id2string(id(e_it));
            if(e_str == "")
                e_str = fill_name(e_it);
            
            std::set<symbol_exprt> dependent_symbols = assignment_symbol_map[lhs];
            std::vector<symbol_exprt> wl;
            wl.assign(dependent_symbols.begin(), dependent_symbols.end());
            while(!wl.empty())
            {
                auto symbol = wl.back();
                wl.pop_back();
                if(assignment_symbol_map.find(symbol) != assignment_symbol_map.end())
                {
                    auto new_symbols = assignment_symbol_map[symbol];
                    for(auto new_symbol : new_symbols)
                    {
                        if(dependent_symbols.find(new_symbol) != dependent_symbols.end())
                            continue;
                        dependent_symbols.insert(new_symbol);
                        wl.push_back(new_symbol);
                    }
                }
            }

            for(auto dependent_symbol : dependent_symbols)
            {
                std::string dependent_symbol_name = dependent_symbol.get_identifier().c_str();
                if(reads.find(dependent_symbol_name) != reads.end())
                    add_necessary_oc_edge(equation, dependent_symbol_name, e_str, "data", true_exprt());
            }
        }
}

void find_offset_symbols(const exprt& expr, std::set<symbol_exprt>& offset_symbols)
{
    if(expr.id() == ID_pointer_offset)
        find_symbols(expr, offset_symbols);
    for(auto& operand : expr.operands())
        find_offset_symbols(operand, offset_symbols);
}

void memory_model_generalt::build_addr(symex_target_equationt &equation, std::set<std::string>& reads)
{
    std::map<symbol_exprt, exprt> assignment_map;
    std::map<symbol_exprt, std::set<symbol_exprt>> assignment_symbol_map;
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_assignment())
        {
            assignment_map[to_symbol_expr(e_it->ssa_lhs)] = e_it->ssa_rhs;
            assignment_symbol_map[to_symbol_expr(e_it->ssa_lhs)] = find_symbols(e_it->ssa_rhs);
        }
    
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_shared_write())
        {
            symbol_exprt lhs = e_it->ssa_lhs;
            if(assignment_map.find(lhs) == assignment_map.end())
                continue;

            std::string e_str = id2string(id(e_it));
            if(e_str == "")
                e_str = fill_name(e_it);
            
            exprt rhs = assignment_map[lhs];
            std::set<symbol_exprt> offset_symbols;
            find_offset_symbols(rhs, offset_symbols);

            std::vector<symbol_exprt> wl;
            wl.assign(offset_symbols.begin(), offset_symbols.end());
            while(!wl.empty())
            {
                auto symbol = wl.back();
                wl.pop_back();
                if(assignment_symbol_map.find(symbol) != assignment_symbol_map.end())
                {
                    auto new_symbols = assignment_symbol_map[symbol];
                    for(auto new_symbol : new_symbols)
                    {
                        if(offset_symbols.find(new_symbol) != offset_symbols.end())
                            continue;
                        offset_symbols.insert(new_symbol);
                        wl.push_back(new_symbol);
                    }
                }
            }

            for(auto offset_symbol : offset_symbols)
            {
                std::string offset_symbol_name = offset_symbol.get_identifier().c_str();
                if(reads.find(offset_symbol_name) != reads.end())
                    add_necessary_oc_edge(equation, offset_symbol_name, e_str, "addr", true_exprt());
            }
        }
}

void memory_model_generalt::build_ctrl(symex_target_equationt &equation, std::set<std::string>& reads)
{
    std::map<symbol_exprt, exprt> guard_map;
    build_guard_map(equation, guard_map);

    std::map<symbol_exprt, std::set<symbol_exprt>> assignment_symbol_map;
    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
        if(e_it->is_assignment())
            assignment_symbol_map[to_symbol_expr(e_it->ssa_lhs)] = find_symbols(e_it->ssa_rhs);

    for(eventst::const_iterator e_it=equation.SSA_steps.begin(); e_it!=equation.SSA_steps.end(); e_it++)
    {
        if(!e_it->is_shared_read() && !e_it->is_shared_write() && !e_it->is_spawn() && !e_it->is_memory_barrier())
            continue;
        
        std::string e_str = id2string(id(e_it));
        if(e_str == "")
            e_str = fill_name(e_it);

        std::set<symbol_exprt> guard_symbols;
        find_symbols(e_it->guard, guard_symbols);

        std::set<symbol_exprt> dependent_symbols;
        for(auto& guard_symbol : guard_symbols)
        {
            if(guard_map.find(guard_symbol) == guard_map.end())
                continue;
            find_symbols(guard_map[guard_symbol], dependent_symbols);
        }

        std::vector<symbol_exprt> wl;
        wl.assign(dependent_symbols.begin(), dependent_symbols.end());
        while(!wl.empty())
        {
            auto symbol = wl.back();
            wl.pop_back();
            if(assignment_symbol_map.find(symbol) != assignment_symbol_map.end())
            {
                auto new_symbols = assignment_symbol_map[symbol];
                for(auto new_symbol : new_symbols)
                {
                    if(dependent_symbols.find(new_symbol) != dependent_symbols.end())
                        continue;
                    dependent_symbols.insert(new_symbol);
                    wl.push_back(new_symbol);
                }
            }
        }

        for(auto dependent_symbol : dependent_symbols)
        {
            std::string dependent_symbol_name = dependent_symbol.get_identifier().c_str();
            if(reads.find(dependent_symbol_name) != reads.end())
                add_necessary_oc_edge(equation, dependent_symbol_name, e_str, "ctrl", true_exprt());
        }
    }
}

void memory_model_generalt::build_lkmm_crit(symex_target_equationt &equation)
{
    for(auto lkmm_lock : lkmm_locks)
        for(auto lkmm_unlock : lkmm_unlocks)
            if(po(lkmm_lock, lkmm_unlock))
                add_necessary_oc_edge(equation, lkmm_lock, lkmm_unlock, "rcu-rscs", true_exprt());
}

#include "memory_model_general_cutter.h"

bool memory_model_generalt::really_need_cutting()
{
    for(auto relation : cat.need_cutting_relations)
        if(cat.base_relations.find(relation) == cat.base_relations.end())
            return true;
    std::cout << "No need for cutting\n";
    return false;
}

void memory_model_generalt::build_cutting(symex_target_equationt &equation)
{
    cuttert cutter(cat, equation.oc_edges, equation.oc_labels);
    cutter.analyze_sets();

    // build base variables for each relation being cut
    for(auto relation : cat.need_cutting_relations)
    {
        if(cat.base_relations.find(relation) != cat.base_relations.end())
            continue;

        // only build variables for derived relations
        int relation_id = cutter.kind_str2id[relation];
        if(cat.get_arity(relation) == 1)
        {
            for(int node_id : cutter.unary_mays[relation_id].elements)
            {
                symbol_exprt bv = nondet_bool_symbol(relation);
                add_necessary_oc_label(equation, cutter.node_id2str[node_id], relation, bv);
            }
        }
        if(cat.get_arity(relation) == 2)
        {
            for(auto node_ids : cutter.binary_mays[relation_id].elements)
            {
                symbol_exprt bv = nondet_bool_symbol(relation);
                add_necessary_oc_edge(equation, cutter.node_id2str[node_ids.first], cutter.node_id2str[node_ids.second], relation, bv);
            }
        }
    }

    cutter.generate_cutting_constraints(equation.oc_edges, equation.oc_labels);

    for(auto& constraint : cutter.cutting_constraints)
    {
        add_constraint(equation, constraint, "cutting", equation.SSA_steps.begin()->source);
    }
}
