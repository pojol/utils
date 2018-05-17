#ifndef _AOI_SCENE_HEADER_
#define _AOI_SCENE_HEADER_

#include <map>
#include <unordered_map>
#include <list>

#include <iostream>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>
#include <set>
#include <iterator>

template <typename IDT, typename POST>
class AoiScene final
{
	typedef std::function<void(IDT, POST, POST)> enter_cb;
	typedef std::function<void(IDT, POST, POST)> move_cb;
	typedef std::function<void(IDT, POST, POST)> leave_cb;

public:
	AoiScene();
	~AoiScene();

	//
	void add(IDT id, POST x, POST y, int distance, enter_cb ecb, move_cb mcb, leave_cb lcb);

	//
	void remove(IDT id);

	//
	void move(IDT id, POST x, POST y);

private:

	struct AoiNod {
		POST x_;
		POST y_;
		IDT id_;
		int radius_;

		typename std::list<std::shared_ptr<AoiNod>>::iterator pos_x_iter;
		typename std::list<std::shared_ptr<AoiNod>>::iterator pos_y_iter;

		enter_cb ecb_;
		move_cb mcb_;
		leave_cb lcb_;

		AoiNod(IDT id, int radius, POST x, POST y, enter_cb ecb, move_cb mcb, leave_cb lcb)
			: id_(id)
			, x_(x)
			, y_(y)
			, radius_(radius)
			, ecb_(ecb)
			, mcb_(mcb)
			, lcb_(lcb)
		{
		}
	};

	typedef std::shared_ptr<AoiNod> AoiNodPtr;

private:
	typedef std::unordered_map<IDT, AoiNodPtr> AoiNodMap;
	typedef std::set<AoiNodPtr> AoiNodSet;
	typedef std::list<AoiNodPtr> AoiNodList;

	void update_impl(AoiNodPtr nodPtr, POST x, POST y);
	void get_range_list(AoiNodPtr nod, AoiNodSet &set);

	void update_list_x(AoiNodPtr nod, POST x);
	void update_list_y(AoiNodPtr nod, POST y);
private:


	AoiNodList nod_x_list_;			
	AoiNodList nod_y_list_;	
	AoiNodMap  nod_map_;	


	AoiNodSet  move_set_;
	AoiNodSet  enter_set_;
	AoiNodSet  leave_set_;
};


template <typename IDT, typename POST>
AoiScene<IDT, POST>::AoiScene()
{

}

template <typename IDT, typename POST>
AoiScene<IDT, POST>::~AoiScene()
{
	nod_x_list_.clear();
	nod_y_list_.clear();
	nod_map_.clear();
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::remove(IDT id)
{
	auto obj_iter = nod_map_.find(id);
	if (obj_iter == nod_map_.end()) {
		return;
	}

	AoiNodPtr nodPtr = obj_iter->second;

	AoiNodSet old_set;
	get_range_list(nodPtr, old_set);
	for (auto &it : old_set)
	{
		nodPtr->lcb_(it->id_, it->x_, it->y_);
		it->lcb_(nodPtr->id_, nodPtr->x_, nodPtr->y_);
	}

	nod_x_list_.erase(nodPtr->pos_x_iter);
	nod_y_list_.erase(nodPtr->pos_y_iter);
	nod_map_.erase(nodPtr->id_);
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::get_range_list(AoiNodPtr nod, AoiNodSet &set)
{
	if (nod->pos_x_iter == nod_x_list_.end() && nod->pos_y_iter == nod_y_list_.end()) {
		return;
	}

	std::set<AoiNodPtr> f_set;

	typename AoiNodList::iterator x_forward_iter = nod->pos_x_iter;
	typename AoiNodList::reverse_iterator x_backward_iter(x_forward_iter);

	for (; x_forward_iter != nod_x_list_.end(); ++x_forward_iter)
	{
		if (abs(nod->x_ - (*x_forward_iter)->x_) > nod->radius_) {
			break;
		}

		if (nod->id_ == (*x_forward_iter)->id_) {
			continue;
		}

		f_set.insert(*x_forward_iter);
	}

	for (; x_backward_iter != nod_x_list_.rend(); ++x_backward_iter)
	{
		if (abs(nod->x_ - (*x_backward_iter)->x_) > nod->radius_) {
			break;
		}

		if (nod->id_ == (*x_backward_iter)->id_) {
			continue;
		}

		f_set.insert(*x_backward_iter);
	}

	typename AoiNodList::iterator y_forward_iter = nod->pos_y_iter;
	typename AoiNodList::reverse_iterator y_backward_iter(y_forward_iter);

	for (; y_forward_iter != nod_y_list_.end(); ++y_forward_iter)
	{
		if (abs(nod->y_ - (*y_forward_iter)->y_) > nod->radius_) {
			break;
		}

		if (nod->id_ == (*y_forward_iter)->id_) {
			continue;
		}

		if (f_set.find(*y_forward_iter) != f_set.end()) {
			set.insert(*y_forward_iter);
		}
	}

	for (; y_backward_iter != nod_y_list_.rend(); ++y_backward_iter)
	{
		if (abs(nod->y_ - (*y_backward_iter)->y_) > nod->radius_) {
			break;
		}

		if (nod->id_ == (*y_backward_iter)->id_) {
			continue;
		}

		if (f_set.find(*y_backward_iter) != f_set.end()) {
			set.insert(*y_backward_iter);
		}
	}
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::move(IDT id, POST x, POST y)
{
	auto obj_iter = nod_map_.find(id);
	if (obj_iter == nod_map_.end()) {
		return;
	}
	AoiNodPtr nodPtr = obj_iter->second;

	update_impl(nodPtr, x, y);
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::update_impl(AoiNodPtr nodPtr, POST x, POST y)
{
	AoiNodSet old_set, new_set;

	get_range_list(nodPtr, old_set);

	update_list_x(nodPtr, x);
	update_list_y(nodPtr, y);

	get_range_list(nodPtr, new_set);

	std::set_intersection(old_set.begin(), old_set.end(), new_set.begin(), new_set.end(), std::inserter(move_set_, move_set_.begin()));
	std::set_difference(new_set.begin(), new_set.end(), move_set_.begin(), move_set_.end(), std::inserter(enter_set_, enter_set_.begin()));
	std::set_difference(old_set.begin(), old_set.end(), move_set_.begin(), move_set_.end(), std::inserter(leave_set_, leave_set_.begin()));

	// call enter
	for (auto &it : enter_set_)
	{
		nodPtr->ecb_(it->id_, it->x_, it->y_);
		it->ecb_(nodPtr->id_, nodPtr->x_, nodPtr->y_);
	}

	// call move
	for (auto &it : move_set_)
	{
		it->mcb_(nodPtr->id_, nodPtr->x_, nodPtr->y_);
	}

	// call leave
	for (auto &it : leave_set_)
	{
		it->lcb_(nodPtr->id_, nodPtr->x_, nodPtr->y_);
		nodPtr->lcb_(it->id_, it->x_, it->y_);
	}

	// clean 
	enter_set_.clear();
	move_set_.clear();
	leave_set_.clear();
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::update_list_y(AoiNodPtr nod, POST y)
{
	int old_y = nod->y_;
	nod->y_ = y;

	if (y != old_y) {

		typename AoiNodList::iterator forward_itr;
		if (nod->pos_y_iter == nod_y_list_.end()) {
			forward_itr = nod_y_list_.begin();
		}
		else {
			forward_itr = nod_y_list_.erase(nod->pos_y_iter);
		}
		typename AoiNodList::reverse_iterator backward_itr(forward_itr);

		typename AoiNodList::iterator pos_itr = nod_y_list_.end();
		if (nod_y_list_.empty()) {
			nod->pos_y_iter = nod_y_list_.insert(nod_y_list_.begin(), nod);
			return;
		}

		if (y > old_y) {
			auto itr = std::find_if(forward_itr, nod_y_list_.end(), [&](typename AoiNodList::value_type it) {
				return std::isless(it->y_, y);
			});

			if (itr != nod_y_list_.end()) {
				pos_itr = itr;
			}
			else {
				pos_itr = --itr;
			}
		}
		else {
			auto ritr = std::find_if(backward_itr, nod_y_list_.rend(), [&](typename AoiNodList::value_type it) {
				return std::isgreater(it->y_, y);
			});

			if (ritr != nod_y_list_.rend()) {
				pos_itr = ritr.base();
			}
			else {
				pos_itr = (--ritr).base();
			}
		}

		nod->pos_y_iter = nod_y_list_.insert(pos_itr, nod);
	}
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::update_list_x(AoiNodPtr nod, POST x)
{
	int old_x = nod->x_;
	nod->x_ = x;

	if (x != old_x) {
		typename AoiNodList::iterator forward_itr;
		if (nod->pos_x_iter == nod_x_list_.end()) {
			forward_itr = nod_x_list_.begin();
		}
		else {
			forward_itr = nod_x_list_.erase(nod->pos_x_iter);
		}
		typename AoiNodList::reverse_iterator backward_itr(forward_itr);

		typename AoiNodList::iterator pos_itr = nod_x_list_.end();

		if (nod_x_list_.empty()) {
			nod->pos_x_iter = nod_x_list_.insert(nod_x_list_.begin(), nod);
			return;
		}

		if (x > old_x) {
			auto itr = std::find_if(forward_itr, nod_x_list_.end(), [&](typename AoiNodList::value_type it) {
				return std::isless(it->x_, x);
			});

			if (itr != nod_x_list_.end()) {
				pos_itr = itr;
			}
			else {
				pos_itr = --itr;
			}
		}
		else {
			auto ritr = std::find_if(backward_itr, nod_x_list_.rend(), [&](typename AoiNodList::value_type it) {
				return std::isgreater(it->x_, x);
			});

			if (ritr != nod_x_list_.rend()) {
				pos_itr = ritr.base();
			}
			else {
				pos_itr = (--ritr).base();
			}
		}

		nod->pos_x_iter = nod_x_list_.insert(pos_itr, nod);

	}
}

template <typename IDT, typename POST>
void AoiScene<IDT, POST>::add(IDT id, POST x, POST y, int distance, enter_cb ecb, move_cb mcb, leave_cb lcb)
{
	if (nod_map_.find(id) != nod_map_.end()) {
		return;
	}

	AoiNodPtr nodPtr = std::make_shared<AoiNod>(id, distance, x, y, ecb, mcb, lcb);
	
	typename AoiNodList::iterator itr = std::find_if(nod_x_list_.begin(), nod_x_list_.end(), [&](typename AoiNodList::value_type it){
		return std::isless(x, it->x_);
	});
	nodPtr->pos_x_iter = nod_x_list_.insert(itr, nodPtr);

	itr = std::find_if(nod_y_list_.begin(), nod_y_list_.end(), [&](typename AoiNodList::value_type it){
		return std::isless(y, it->y_);
	});
	nodPtr->pos_y_iter = nod_y_list_.insert(itr, nodPtr);

	nod_map_.insert(std::make_pair(id, nodPtr));

	AoiNodSet new_set;
	get_range_list(nodPtr, new_set);
	for (auto &it : new_set)
	{
		nodPtr->ecb_(it->id_, it->x_, it->y_);
		it->ecb_(nodPtr->id_, nodPtr->x_, nodPtr->y_);
	}
}


#endif // !_AOI_SCENE_HEADER_