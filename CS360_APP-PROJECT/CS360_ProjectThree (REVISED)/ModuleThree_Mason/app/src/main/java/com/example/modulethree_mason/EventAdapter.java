package com.example.modulethree_mason;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Button;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class EventAdapter extends RecyclerView.Adapter<EventAdapter.EventViewHolder> {

    private List<Event> eventList;

    public EventAdapter(List<Event> eventList) {
        this.eventList = eventList;
    }

    @NonNull
    @Override
    public EventViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.event_item, parent, false);
        return new EventViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull EventViewHolder holder, int position) {
        Event event = eventList.get(position);
        holder.txtName.setText(event.getName());
        holder.txtDate.setText(event.getDate());
        holder.txtCategory.setText(event.getCategory());

        // Set color based on category
        switch (event.getCategory().toLowerCase()) {
            case "work":
                holder.itemView.setBackgroundColor(holder.itemView.getResources().getColor(R.color.workColor));
                break;
            case "health":
                holder.itemView.setBackgroundColor(holder.itemView.getResources().getColor(R.color.healthColor));
                break;
            case "social":
                holder.itemView.setBackgroundColor(holder.itemView.getResources().getColor(R.color.socialColor));
                break;
            default:
                holder.itemView.setBackgroundColor(holder.itemView.getResources().getColor(android.R.color.white));
                break;
        }

        // Delete button listener
        holder.btnDelete.setOnClickListener(v -> {
            int pos = holder.getAdapterPosition();
            if (pos != RecyclerView.NO_POSITION) {
                eventList.remove(pos);
                notifyItemRemoved(pos);
            }
        });
    }

    @Override
    public int getItemCount() {
        return eventList.size();
    }

    static class EventViewHolder extends RecyclerView.ViewHolder {
        TextView txtName, txtDate, txtCategory;
        Button btnDelete; // new delete button

        public EventViewHolder(@NonNull View itemView) {
            super(itemView);
            txtName = itemView.findViewById(R.id.txtEventName);
            txtDate = itemView.findViewById(R.id.txtEventDate);
            txtCategory = itemView.findViewById(R.id.txtEventCategory);
            btnDelete = itemView.findViewById(R.id.btnDelete); // link delete button
        }
    }
}