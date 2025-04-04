#pragma once
#include "string"
#include "iostream"
#include "unordered_map"

class Type {
	public:
		virtual ~Type() = default;
		virtual void print() const = 0;
};
	
	class PrimitiveType : public Type {
	public:
		std::string name;  // e.g., "int", "float"
		
		PrimitiveType(const std::string& name) : name(name) {}
	
		void print() const override {
			std::cout << "PrimitiveType(" << name << ")" << std::endl;
		}
	};
	
	class StructType : public Type {
	public:
		std::string name;  // Name of the struct
		std::unordered_map<std::string, Type*> fields; // Field names + types
	
		StructType(const std::string& name) : name(name) {}
	
		void addField(const std::string& fieldName, Type* fieldType) {
			fields[fieldName] = fieldType;
		}
	
		void print() const override {
			for (auto& field : fields) {
				std::cout << "  " << field.first << ": ";
				field.second->print();
			}
		}
	};
	