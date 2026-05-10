#pragma once
#include <iostream>
using namespace std;

template<typename T>
class Storage {
private:
	T* arr;
	int currentOccupied;
	int maxCapacity;
public:
	Storage(int capacity = 100) {
		this->maxCapacity = (capacity > 0) ? capacity : 100;
		this->currentOccupied = 0;
		this->arr = new T[this->maxCapacity];
	}

	Storage(const Storage& rhs) {
		this->maxCapacity = rhs.maxCapacity;
		this->currentOccupied = rhs.currentOccupied;
		this->arr = new T[this->maxCapacity];
		for (int i = 0; i < this->currentOccupied; i++) {
			this->arr[i] = rhs.arr[i];
		}
	}

	bool add(const T& item) {
		if (this->currentOccupied >= this->maxCapacity) {
			cout << "Storage Reached Maximum Capacity...\n";
			return false;
		}
		this->arr[this->currentOccupied] = item;
		this->currentOccupied++;
		return true;
	}

	bool removeByID(int id) {
		for (int i = 0; i < this->currentOccupied; i++) {
			if (this->arr[i].getID() == id) {
				for (int j = i; j < this->currentOccupied - 1; j++) {
					this->arr[j] = this->arr[j + 1];
				}
				this->currentOccupied--;
				return true;
			}
		}
		return false;
	}

	T* findByID(int id) {
		for (int i = 0; i < this->currentOccupied; i++) {
			if (this->arr[i].getID() == id) {
				return &this->arr[i];
			}
		}
		return nullptr;
	}

	T* getAll() { return this->arr; }
	const T* getAll() const { return this->arr; }
	int getSize() const { return this->currentOccupied; }
	void clear() { this->currentOccupied = 0; }

	Storage& operator=(const Storage& rhs) {
		if (&rhs == this) {
			return *this;
		}
		delete[] this->arr;
		this->maxCapacity = rhs.maxCapacity;
		this->currentOccupied = rhs.currentOccupied;
		this->arr = new T[this->maxCapacity];
		for (int i = 0; i < this->currentOccupied; i++) {
			this->arr[i] = rhs.arr[i];
		}
		return *this;
	}

	~Storage() {
		delete[] this->arr;
		this->arr = nullptr;
	}
};
